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
	int file;
	struct stat statbuf;
	filesystem_t *private_data;
	private_data = malloc(sizeof(filesystem_t));
	//int pagesize = sysconf(_SC_PAGE_SIZE);
	char path[50];

	if (argc < 2) {
		printf("Uso: %s <nombre_archivo>\n", argv[0]);
		return 1;
	}

	strcpy(path, "../FUSE2/");
	strcat(path, argv[1]);

	file = open(path, O_RDWR);
	//file = open("../FUSE/filesystem.img", O_RDWR);
	if (file == -1) {
	   perror("Error al abrir el archivo");
	   exit(EXIT_FAILURE);
	}
	if(fstat(file, &statbuf) == -1){
		perror("stat");
		exit(EXIT_FAILURE);
	}

	private_data->superblock = (struct superblock_fs *) mmap(NULL,sizeof(struct superblock_fs),PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
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

	private_data->superblock->inodes_ocupados = 0;
	 //movemos el puntero del fichero a donde apunta el offset
	 unsigned long bitmap_offset = private_data->superblock->bitmapb_offset;
	 unsigned long numb = private_data->superblock->num_blocks/8, numi = private_data->superblock->num_inodes/8;
	 private_data->block_bitmap.array = (unsigned char*) mmap(NULL,numb,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 private_data->block_bitmap.size = numb;

	 bitmap_offset = private_data->superblock->bitmapi_offset;

	 private_data->inode_bitmap.array = (unsigned char*) mmap(NULL,numi,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 private_data->inode_bitmap.size = numi;


 	 private_data->inode = (struct inode_fs*) mmap(NULL,(sizeof(struct inode_fs) * private_data->superblock->num_inodes), PROT_WRITE|PROT_READ, MAP_SHARED,file, private_data->superblock->offset_inodos); // @suppress("Field cannot be resolved")
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

		 create_root(private_data);
		 
}
