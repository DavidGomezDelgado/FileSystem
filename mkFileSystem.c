#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include "filesystem.h"

#define MAGIC_N 123456

//structura bloque de datos
typedef char block_t[][BLOCK_SIZE];

struct superblock_fs {
	unsigned long magic_number;
	unsigned long bitmapb_offset;
	unsigned long bitmapi_offset;
    unsigned long num_blocks;
    unsigned long num_inodes;
    unsigned long offset_inodos;
    unsigned long offset_bloques;
    unsigned long block_size;
};

int main(int argc, char *argv[]){
	int file;
	struct stat statbuf;
	struct superblock_fs *superblock;
	struct bitmap_t bitmapb;
	struct bitmap_t bitmapi;
	struct inode_fs *inodos;
	struct block_t *bloques;
	unsigned long nb = 1;
	int pagesize = sysconf(_SC_PAGE_SIZE);

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

	superblock = mmap(NULL,sizeof(struct superblock_fs),PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
	 if (superblock == MAP_FAILED) {
		 perror("Error al mapear el archivo");
		 close(file);
		 exit(EXIT_FAILURE);
	 }
	 superblock->magic_number = MAGIC_N;
	 //NUMERO DE BLOQUES TOTAL
	 superblock->bitmapb_offset = BLOCK_SIZE;
	 superblock->block_size = BLOCK_SIZE;
	 superblock->num_blocks =  statbuf.st_size/BLOCK_SIZE;//NUMERO DE BLOQUES TOTAL
	 superblock->num_inodes = superblock->num_blocks/4;
	 if(superblock->num_blocks%BLOCK_SIZE == 0 && superblock->num_blocks != BLOCK_SIZE){
		 superblock->bitmapi_offset = superblock->bitmapb_offset + BLOCK_SIZE*(superblock->num_blocks/BLOCK_SIZE);
	 }else{
		 superblock->bitmapi_offset = superblock->bitmapb_offset + BLOCK_SIZE*(1+superblock->num_blocks/BLOCK_SIZE);
	 }

	 //Guardamos los inodos en un array de caracteres, cada caracter es 1 byte que representa a 8 inodos
	 if((superblock->num_inodes/8)%BLOCK_SIZE == 0 && superblock->num_inodes != BLOCK_SIZE){
		 superblock->offset_inodos = superblock->bitmapi_offset + (superblock->num_inodes/8)/BLOCK_SIZE;
	 }else{
		 superblock->offset_inodos = superblock->bitmapi_offset + (superblock->num_inodes/8)/BLOCK_SIZE+1;
	 }
	 if((sizeof(struct inode_fs) * superblock->num_inodes)%BLOCK_SIZE == 0){
		 superblock->offset_bloques = superblock->offset_inodos + (sizeof(struct inode_fs) * superblock->num_inodes)/BLOCK_SIZE;
	 }else{
		 superblock->offset_bloques = superblock->offset_inodos + ((sizeof(struct inode_fs) * superblock->num_inodes)/BLOCK_SIZE) +1;
	 }

	 //movemos el puntero del fichero a donde apunta el offset
	 unsigned long bitmap_offset = superblock->bitmapb_offset;
	 if(superblock->num_blocks%8 != 0){
		 nb = superblock->num_blocks/8 +1;
	 }
	 bitmapb.array = (char*) mmap(NULL,nb,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 bitmapb.size = superblock->num_blocks/8;
	 //Seteamos a 1 tantos bits como bloques tengamos ocupados
	 //Bloques ocupados: (1)Superbloque + ()Bitmapb + ()Bitmapi + num_inodos
	 uint8_t i;
	 uint8_t numb = superblock->num_blocks/8, numi = superblock->num_inodes/8;
	 if(numb%BLOCK_SIZE!= 0){
	 	numb = numb/BLOCK_SIZE + 1;
	 }
	 if(numi%BLOCK_SIZE != 0){
	 	numi = numi/BLOCK_SIZE + 1;
	 }
	 unsigned long bloques_ocupados = 1 + numb+ numi + superblock->num_inodes;
	 int control = 0;
	 while(i < bloques_ocupados && (control !=-1)){
		 control = setBit(&bitmapb);
		 if(control == -1){
			 printf("Error, no hay espacio");
			 exit(EXIT_FAILURE);
		 }
		 i++;
	 }

	 bitmap_offset = superblock->bitmapi_offset;

	 nb = 1;
	 if(superblock->num_inodes%8 != 0){
	 	nb = superblock->num_inodes/8 +1;
	 }
	 bitmapi.array = (unsigned char*) mmap(NULL,nb,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 bitmapi.size = superblock->num_inodes/8;
	 i = 0;
	setBit(&bitmapi);


	 inodos = (struct inode_fs*) mmap(NULL,superblock->offset_bloques-superblock->offset_inodos, PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->offset_inodos);
	 bloques = (struct block_t*) mmap(NULL,statbuf.st_size-superblock->offset_bloques, PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->offset_bloques);

	printf("Todo guardado\n");

	close(file);

	file = open("filesystem.txt", O_CREAT | O_RDWR, 0664);
	printf("%lu\n,%lu\n,%lu\n,%lu\n, %lu\n, %lu\n",superblock->magic_number, superblock->num_blocks, superblock->num_inodes, superblock->block_size, superblock->bitmapb_offset, superblock->bitmapi_offset);
//	fflush(stdout);
//	for(i = 0; i < bitmapb.size; i++){
//		printf("%c", bitmapb.array[i]);
//	}
//	fflush(stdout);
	for(i = 0; i < bitmapi.size; i++){
			printf("%c", bitmapi.array[i]);
	}
	fflush(stdout);
	close(file);
}
