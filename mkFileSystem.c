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

//estructura que reutilizamos para bitmapb y bitmapi
typedef struct bitmap_t{
	char *array;
	uint64_t size;
};

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

	superblock = mmap(NULL,BLOCK_SIZE,PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
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
	 if(superblock->num_blocks%BLOCK_SIZE == 0){
		 superblock->bitmapi_offset = superblock->bitmapb_offset + superblock->num_blocks/BLOCK_SIZE;
	 }else{
		 superblock->bitmapi_offset = superblock->bitmapb_offset + superblock->num_blocks/BLOCK_SIZE+1;
	 }
	 if(superblock->num_inodes%BLOCK_SIZE == 0){
		superblock->offset_inodos = superblock->bitmapi_offset + superblock->num_inodes;
	 }else{
	 	superblock->offset_inodos = superblock->bitmapi_offset + superblock->num_inodes+1;
	 }

	 if((sizeof(struct inode_fs) * superblock->num_inodes)%BLOCK_SIZE == 0){
	 	superblock->offset_bloques = superblock->offset_inodos + sizeof(struct inode_fs) * superblock->num_inodes;
	 }else{
	 	 superblock->offset_bloques = superblock->offset_inodos + sizeof(struct inode_fs) * superblock->num_inodes+1;
	 }

	 //movemos el puntero del fichero a donde apunta el offset
	 unsigned long bitmap_offset = superblock->bitmapb_offset;

	 bitmapb.array = (unsigned char*) mmap(NULL,superblock->bitmapi_offset - bitmap_offset,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 bitmapb.size = superblock->num_blocks;

	bitmap_offset = superblock->bitmapi_offset;

	 bitmapi.array = (unsigned char*) mmap(NULL,superblock->offset_inodos-bitmap_offset,PROT_WRITE|PROT_READ, MAP_SHARED,file, bitmap_offset);
	 bitmapi.size = superblock->num_inodes;

	 inodos = (struct inode_fs*) mmap(NULL,superblock->offset_bloques-superblock->offset_inodos, PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->offset_inodos);
	 bloques = (struct block_t*) mmap(NULL,statbuf.st_size-superblock->offset_bloques, PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->offset_bloques);

	printf("Todo guardado\n");

	close(file);

	file = open("filesystem.txt", O_CREAT | O_RDWR, 0664);
	printf("%lu\n,%lu\n,%lu\n,%lu\n, %lu\n, %lu\n",superblock->magic_number, superblock->num_blocks, superblock->num_inodes, superblock->block_size, superblock->bitmapb_offset, superblock->bitmapi_offset);

	close(file);
}





/*#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../filesystem.h"

#define NUM_BLOCKS 1048576 //(4GB)
#define NUM_INODES 1000

#define BLOCK_SIZE 4096

struct superblock_fs {
    int num_blocks;
    int num_inodes;
    int block_size;
    struct block_bitmap_fs block_bitmap;
    struct inode_bitmap_fs inode_bitmap;
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Debe proporcionar el nombre del archivo para almacenar el superbloque.\n");
		return 1;
	}
	
	// Inicializar el superbloque
	struct superblock_fs superblock;
	memset(&superblock, 0, sizeof(struct superblock_fs));
	superblock.num_blocks = NUM_BLOCKS;
	superblock.num_inodes = NUM_INODES;
	superblock.block_size = BLOCK_SIZE;

	// Inicializar el mapa de bits de bloques y de inodos
	memset(&superblock.block_bitmap, 0, sizeof(struct block_bitmap_fs));
	memset(&superblock.inode_bitmap, 0, sizeof(struct inode_bitmap_fs));

	// Crear el inodo raíz
	struct inode_fs *root_inode = create_root(&superblock.inode_bitmap, &superblock.block_bitmap);

	// Escribir el superbloque en un archivo o dispositivo
	FILE *file = fopen(argv[1], "w");
	if (file == NULL) {
    		printf("Error al abrir el archivo.\n");
    		return 1;
	}

	fprintf(file, "%d\n", superblock.num_blocks);
	fprintf(file, "%d\n", superblock.num_inodes);
	fprintf(file, "%d\n", superblock.block_size);

	for (int i = 0; i < NUM_BLOCKS / 8; i++) {
	    fprintf(file, "%02X ", superblock.block_bitmap.bitmap[i]);
	    fprintf(file, "\n");
	}

	for (int i = 0; i < NUM_INODES / 8; i++) {
	    fprintf(file, "%02X ", superblock.inode_bitmap.bitmap[i]);
	    fprintf(file, "\n");
	}

	fclose(file);

	printf("Superbloque inicializado correctamente.\n");

	return 0;
}*/
