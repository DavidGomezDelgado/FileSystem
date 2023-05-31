#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include "filesystem.h"

int main(int argc, char *argv[]){
	int file;
	struct stat statbuf;
	//private_data private_data = malloc(sizeof(filesystem_t));
	filesystem_t myfilesystem;
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

	myfilesystem.superblock = mmap(NULL,sizeof(struct superblock_fs),PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
	 if (myfilesystem.superblock == MAP_FAILED) {
		 perror("Error al mapear el archivo");
		 close(file);
		 exit(EXIT_FAILURE);
	 }
	 myfilesystem.superblock->magic_number = MAGIC_N;
	 //NUMERO DE BLOQUES TOTAL
	 myfilesystem.superblock->block_size = BLOCK_SIZE;
	 myfilesystem.superblock->num_blocks =  statbuf.st_size/BLOCK_SIZE;//NUMERO DE BLOQUES TOTAL

	 //Para el número de bloques total que tenemos por inodo, vamos a multiplicarlo por el número de inodos que hay en un bloque
	 myfilesystem.superblock->num_inodes = (myfilesystem.superblock->num_blocks/4)*(BLOCK_SIZE/sizeof(struct inode_fs));

	 //****VAMOS A CALCULAR LOS OFFSET****//

	 myfilesystem.superblock->bitmapb_offset = BLOCK_SIZE;

	 if((myfilesystem.superblock->num_blocks/8)%BLOCK_SIZE == 0){ // -> NUMERO DE BLOQUES QUE VAMOS A OCUPAR
		 myfilesystem.superblock->bitmapi_offset = myfilesystem.superblock->bitmapb_offset + (myfilesystem.superblock->num_blocks/8);
	 }else{
		 myfilesystem.superblock->bitmapi_offset = myfilesystem.superblock->bitmapb_offset + (((myfilesystem.superblock->num_blocks/8)/BLOCK_SIZE)+1)*BLOCK_SIZE;
	 }

	 //Guardamos los inodos en un array de caracteres, cada caracter es 1 byte que representa a 8 inodos
	 if((myfilesystem.superblock->num_inodes/8)%BLOCK_SIZE == 0){
		 myfilesystem.superblock->offset_inodos = myfilesystem.superblock->bitmapi_offset + (myfilesystem.superblock->num_inodes/8);
	 }else{
		 myfilesystem.superblock->offset_inodos = myfilesystem.superblock->bitmapi_offset + (((myfilesystem.superblock->num_inodes/8)/BLOCK_SIZE)+1) * BLOCK_SIZE;
	 }
	 if((sizeof(struct inode_fs) * myfilesystem.superblock->num_inodes)%BLOCK_SIZE == 0){
		 myfilesystem.superblock->offset_bloques = myfilesystem.superblock->offset_inodos + (sizeof(struct inode_fs) * myfilesystem.superblock->num_inodes);
	 }else{
		 myfilesystem.superblock->offset_bloques = myfilesystem.superblock->offset_inodos + (((sizeof(struct inode_fs) * myfilesystem.superblock->num_inodes)/BLOCK_SIZE) +1)*BLOCK_SIZE;
	 }

	 //movemos el puntero del fichero a donde apunta el offset
	 unsigned long bitmap_offset = myfilesystem.superblock->bitmapb_offset;
	 unsigned long numb = myfilesystem.superblock->num_blocks/8, numi = myfilesystem.superblock->num_inodes/8;
	 myfilesystem.block_bitmap.array = (unsigned char*) mmap(NULL,numb,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 myfilesystem.block_bitmap.size = numb;

	 //**Pruebas bitmapb**//
	 //Seteamos a 1 tantos bits como bloques tengamos ocupados
	 //Bloques ocupados: (1)Superbloque + ()Bitmapb + ()Bitmapi + num_inodos

	 unsigned long i;
	 unsigned long nb, ni;
	 if(numb%BLOCK_SIZE!= 0){
	 	 	 nb = (numb/BLOCK_SIZE + 1);
	 	 }
	 if(numi%BLOCK_SIZE != 0){
	 	 ni = (numi/BLOCK_SIZE + 1);
	 }
	 unsigned long bloques_ocupados = 1 + nb + ni + myfilesystem.superblock->num_inodes;
	 int control = 0;
	 while(i < bloques_ocupados && (control !=-1)){
		 control = free_bit(&(myfilesystem.block_bitmap));
		 if(control == -1){
			 printf("Error, no hay espacio");
			 exit(EXIT_FAILURE);
		 }
		 i++;
	 }
	 //**FIN DE PRUEBAS**//

	 bitmap_offset = myfilesystem.superblock->bitmapi_offset;

	 myfilesystem.inode_bitmap.array = (unsigned char*) mmap(NULL,numi,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 myfilesystem.inode_bitmap.size = numi;

	 //**MAS TESTSS**//
//	i = 0;
//	setBit(private_data.inode_bitmap);
	//**FIN TESTS**//

	 myfilesystem.inode = (struct inode_fs*) mmap(NULL,myfilesystem.superblock->bitmapb_offset-myfilesystem.superblock->bitmapi_offset, PROT_WRITE|PROT_READ, MAP_SHARED,file, myfilesystem.superblock->offset_inodos); // @suppress("Field cannot be resolved")
	 myfilesystem.block = (block_t*) mmap(NULL,statbuf.st_size-myfilesystem.superblock->offset_bloques, PROT_WRITE|PROT_READ, MAP_SHARED,file, myfilesystem.superblock->offset_bloques);

	printf("Todo guardado\n");

	close(file);

	file = open("filesystem.txt", O_CREAT | O_RDWR, 0664);
	//printf("%lu\n,%lu\n,%lu\n,%lu\n, %lu\n, %lu\n",superblock->magic_number, superblock->num_blocks, superblock->num_inodes, superblock->block_size, superblock->bitmapb_offset, superblock->bitmapi_offset);
	fflush(stdout);
	for(i = 0; i < myfilesystem.block_bitmap.size; i++){
		printf("%#x\n", myfilesystem.block_bitmap.array[i]);
	}
//	fflush(stdout);
//	for(i = 0; i < bitmapi.size; i++){
//			printf("%c", bitmapi.array[i]);
//	}
	fflush(stdout);
	close(file);
}
