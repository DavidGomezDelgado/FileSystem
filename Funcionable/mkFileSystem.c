#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "filesystem.h"

int main(int argc, char *argv[]){

	printf("inodo: %d\n", sizeof(struct inode_fs));
	int file;
	struct stat statbuf;
	filesystem_t *private_data;
	private_data = malloc(sizeof(filesystem_t));
	//int pagesize = sysconf(_SC_PAGE_SIZE);

	//file = open(argv[1], O_RDWR);
	file = open("filesystem.img", O_RDWR);
	if (file == -1) {
	   perror("Error al abrir el archivo");
	   exit(EXIT_FAILURE);
	}
	if(fstat(file, &statbuf) == -1){
		perror("stat");
		exit(EXIT_FAILURE);
	}



	private_data->superblock = mmap(NULL,sizeof(struct superblock_fs),PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
	 if (private_data->superblock == MAP_FAILED) {
		 perror("Error al mapear el archivo");
		 close(file);
		 exit(EXIT_FAILURE);
	 }
	 private_data->superblock->magic_number = MAGIC_N;
	 //NUMERO DE BLOQUES TOTAL
	 private_data->superblock->block_size = BLOCK_SIZE;
	 private_data->superblock->num_blocks =  statbuf.st_size/BLOCK_SIZE;//NUMERO DE BLOQUES TOTAL

	 //Para el número de bloques total que tenemos por inodo, vamos a multiplicarlo por el número de inodos que hay en un bloque
	 private_data->superblock->num_inodes = (private_data->superblock->num_blocks/4)*(BLOCK_SIZE/sizeof(struct inode_fs));

	 //****VAMOS A CALCULAR LOS OFFSET****//

	 private_data->superblock->bitmapb_offset = BLOCK_SIZE;

	 if((private_data->superblock->num_blocks/8)%BLOCK_SIZE == 0){ // -> NUMERO DE BLOQUES QUE VAMOS A OCUPAR
		 private_data->superblock->bitmapi_offset = private_data->superblock->bitmapb_offset + (private_data->superblock->num_blocks/8);
	 }else{
		 private_data->superblock->bitmapi_offset = private_data->superblock->bitmapb_offset + (((private_data->superblock->num_blocks/8)/BLOCK_SIZE)+1)*BLOCK_SIZE;
	 }

	 //Guardamos los inodos en un array de caracteres, cada caracter es 1 byte que representa a 8 inodos
	 if((private_data->superblock->num_inodes/8)%BLOCK_SIZE == 0){
		 private_data->superblock->offset_inodos = private_data->superblock->bitmapi_offset + (private_data->superblock->num_inodes/8);
	 }else{
		 private_data->superblock->offset_inodos = private_data->superblock->bitmapi_offset + (((private_data->superblock->num_inodes/8)/BLOCK_SIZE)+1) * BLOCK_SIZE;
	 }
	 if((sizeof(struct inode_fs) * private_data->superblock->num_inodes)%BLOCK_SIZE == 0){
		 private_data->superblock->offset_bloques = private_data->superblock->offset_inodos + (sizeof(struct inode_fs) * private_data->superblock->num_inodes);
	 }else{
		 private_data->superblock->offset_bloques = private_data->superblock->offset_inodos + (((sizeof(struct inode_fs) * private_data->superblock->num_inodes)/BLOCK_SIZE) +1)*BLOCK_SIZE;
	 }

	 //movemos el puntero del fichero a donde apunta el offset
	 unsigned long bitmap_offset = private_data->superblock->bitmapb_offset;
	 unsigned long numb = private_data->superblock->num_blocks/8, numi = private_data->superblock->num_inodes/8;
	 private_data->block_bitmap.array = (unsigned char*) mmap(NULL,numb,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 private_data->block_bitmap.size = numb;

	 bitmap_offset = private_data->superblock->bitmapi_offset;

	 private_data->inode_bitmap.array = (unsigned char*) mmap(NULL,numi,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 private_data->inode_bitmap.size = numi;


 	 private_data->inode = (struct inode_fs*) mmap(NULL,private_data->superblock->bitmapi_offset - private_data->superblock->bitmapb_offset, PROT_WRITE|PROT_READ, MAP_SHARED,file, private_data->superblock->offset_inodos); // @suppress("Field cannot be resolved")
	 private_data->block = (block_t*) mmap(NULL,statbuf.st_size-private_data->superblock->offset_bloques, PROT_WRITE|PROT_READ, MAP_SHARED,file, private_data->superblock->offset_bloques);

	printf("Todo guardado\n");

	close(file);

	 //**Pruebas bitmapb**//
		 //Seteamos a 1 tantos bits como bloques tengamos ocupados
		 //Bloques ocupados: (1)Superbloque + ()Bitmapb + ()Bitmapi + num_inodos

		 unsigned long i = 0;
		 unsigned long nb, ni;
		 if(numb%BLOCK_SIZE!= 0){
		 	 	 nb = (numb/BLOCK_SIZE + 1);
		 	 }
		 if(numi%BLOCK_SIZE != 0){
		 	 ni = (numi/BLOCK_SIZE + 1);
		 }
		 unsigned long bloques_ocupados = 1 + nb + ni + (private_data->superblock->num_inodes/sizeof(struct inode_fs));
		 private_data->superblock->reserved_block = bloques_ocupados;
		 int control = 0;
		 while(i < bloques_ocupados && (control !=-1)){
			 control = free_bit(&(private_data->block_bitmap));
			 if(control == -1){
				 printf("Error, no hay espacio");
				 exit(EXIT_FAILURE);
			 }
			 i++;
		 }
/*
		 struct inode_fs *root = create_root(private_data);
	//FIN DE PRUEBAS//


	//BORRAR SOLO SON PRUEBAS//
	file = open("filesystem.txt", O_CREAT | O_RDWR, 0664);
	fflush(stdout);
	for(i = 0; i < private_data->block_bitmap.size; i++){
		printf("%#x\n", private_data->block_bitmap.array[i]);
	}
	printf("\n%d \n, %c,\n %d\n", private_data->inode->i_num, private_data->inode->i_type, private_data->inode->i_tam);
	fflush(stdout);
	struct directory_entry *aux = (struct directory_entry*) private_data->block[private_data->inode[0].i_directos[0] - private_data->superblock->reserved_block];
	for(i = 0; i < 2; i++){
		printf("%s %d\n", aux[i].name, aux[i].inode->i_num);
	}

//	private_data private;
//
//	private.superblock = mmap(NULL, sizeof(struct superblock_fs), PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
//	private.block_bitmap.array = mmap(NULL, private.superblock->num_blocks/8, PROT_WRITE|PROT_READ, MAP_SHARED,file, private.superblock->bitmapb_offset);
//	private.block_bitmap.size = private.superblock->num_blocks;
//	private.inode_bitmap.array = mmap(NULL, private.superblock->num_inodes/8, PROT_WRITE|PROT_READ, MAP_SHARED,file, private.superblock->bitmapi_offset);
//	private.inode_bitmap.size = private.superblock->num_inodes;
//	private.inode = mmap(NULL,private.superblock->bitmapi_offset - private.superblock->bitmapb_offset, PROT_WRITE|PROT_READ, MAP_SHARED,file, private.superblock->offset_inodos);
//	private.block = (block_t*) mmap(NULL,statbuf.st_size-private.superblock->offset_bloques, PROT_WRITE|PROT_READ, MAP_SHARED,file, private.superblock->offset_bloques);
//	private.fd = file;
//
	create_inode('d', private_data);
	printf("\n%d \n, %c,\n %d\n", private_data->inode[1].i_num, private_data->inode[1].i_type, private_data->inode[1].i_tam);
	fflush(stdout);
	struct directory_entry *aux2 = (struct directory_entry*) private_data->block[private_data->inode[1].i_directos[0] - private_data->superblock->reserved_block];
	for(i = 0; i < 2; i++){
		printf("%s %d\n", aux2[i].name, aux2[i].inode->i_num);
	}

	create_inode('f', private_data);
	create_inode('f', private_data);

	for (i = 0; i < 10; i++) {
		printf("Byte %ld: %#x\n", i, private_data->inode_bitmap.array[i]);
	}

	// ¡¡SE CREAN BIEN LOS INODOS!!
	printf("Mostramos id de inodos: \n");
	for (i = 0; i < 10; i++) {
		printf("id: %d\n", private_data->inode[i].i_num);
	}

	touch("fichero1", "/" , private_data);
	touch("fichero2", "/" , private_data);
	make_dir("directorio1", "/", private_data);

	// Buscamos inodo de directorio1
	struct inode_fs *dir1 = inode_search_path("/directorio1", private_data);

	touch("fichero3", "/directorio1", private_data);
	touch("fichero4", "/directorio1", private_data);
	touch("fichero5", "/directorio1", private_data);
	touch("fichero6", "/directorio1", private_data);

	// probamos para un fichero existente
	touch("fichero5", "/directorio1/", private_data);

	printf("Mostramos bitmap de inodos\n");
	for (i = 0; i < 10; i++) {
		printf("Byte %ld: %#x\n", i, private_data->inode_bitmap.array[i]);
	}
/*
	printf("Mostramos contenido de /: \n");
	aux2 = (struct directory_entry *) private_data->block[root->i_directos[0] - private_data->superblock->reserved_block];
	i = 0;
	while (i < 10 && aux2[i].inode != NULL) {
		printf(" nombre: %s id: %d\n", aux2[i].name, aux2[i].inode->i_num);
		i++;
	}
*/

//	read_directory("/", root, private_data);

//	fflush(stdout);
/*
	printf("Mostramos contenido de directorio1: \n");
	aux2 = (struct directory_entry *) private_data->block[dir1->i_directos[0] - private_data->superblock->reserved_block];
	i = 0;
	while (i < 10 && aux2[i].inode != NULL) {
		printf(" nombre: %s id: %d\n", aux2[i].name, aux2[i].inode->i_num);
		i++;
	}
*/
//	read_directory("directorio1", dir1, private_data);

//	fflush(stdout);
/*
	rm("fichero1", root, private_data);
	read_directory("/", root, private_data);

	printf("Compruebo si se guardan bien los inodos en private_data:\n");
	for(i = 0; i < 20; i++) {
		printf("id: %d ", private_data->inode[i].i_num);
	}
	printf("Se guardan bien :D\n");

	file_edit("Hola", "fichero2", root, private_data);
	char *cadena = read_file("fichero2", root, private_data);
	printf("La cadena es:\n%s\n",cadena);
	file_edit(":)", "fichero2", root, private_data);
	char *cadena2 = read_file("fichero2", root, private_data);
	printf("La cadena es:\n%s\n",cadena2);

	make_dir("directorio2", "/", private_data);
	struct inode_fs *inodo_aux = inode_search_path("/directorio2", private_data);
	if(inodo_aux == NULL){
		printf("No existe");
	}else{
		printf("%d %c\n", inodo_aux->i_num, inodo_aux->i_type);
	}
	fflush(stdout);
	close(file);
	*/
	//HASTA AQUÍ//
}
