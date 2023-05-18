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

struct superblock_fs {
	unsigned long magic_number;
	unsigned long bitmapb_offset;
	unsigned long bitmapi_offset;
    unsigned long num_blocks;
    unsigned long num_inodes;
    unsigned long block_size;
    char relleno [BLOCK_SIZE-40];
};

int main(){
	int file, nBloquesReservados = 1;
	struct superblock_fs *superblock;
	struct block_bitmap_fs *bitmapb;
	struct inode_bitmap_fs *bitmapi;
	printf("%d\n", sizeof(struct superblock_fs));

	file = open("filesystem.txt", O_CREAT | O_RDWR | O_TRUNC, 0664);
	if (file == -1) {
	   perror("Error al abrir el archivo");
	   return 1;
	}

	//Hacemos que el fichero tenga el tamaño máximo posible y ponemos al final del fichero el caracter vacío
	ftruncate(file, BLOCK_SIZE*NUM_BLOCKS);
	write(file, "\0", 1);

	superblock = mmap(NULL,BLOCK_SIZE,PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
	 if (superblock == MAP_FAILED) {
		 perror("Error al mapear el archivo");
		 close(file);
		 return 1;
	 }
	 superblock->magic_number = MAGIC_N;
	 superblock->bitmapb_offset = BLOCK_SIZE;
	 superblock->block_size = BLOCK_SIZE;
	 superblock->num_blocks = NUM_BLOCKS;
	 superblock->num_inodes = NUM_INODES;
	 printf("%lu\n,%lu\n,%lu\n,%lu\n, %lu\n",superblock->magic_number, superblock->num_blocks, superblock->num_inodes, superblock->block_size, superblock->bitmapb_offset);

	 //movemos el puntero del fichero a donde apunta el offset
	 unsigned long bitmap_offset = superblock->bitmapb_offset;
	 lseek(file, bitmap_offset, SEEK_SET);

	 if(BLOCK_SIZE < sizeof(bitmapb->bitmap)){
		 nBloquesReservados = sizeof(bitmapb->bitmap)/BLOCK_SIZE;
	 }
//
	 bitmapb = mmap(NULL,BLOCK_SIZE*nBloquesReservados,PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->bitmapb_offset);
	 printf("%d\n", sizeof(bitmapb->bitmap));

	 superblock->bitmapi_offset = BLOCK_SIZE*nBloquesReservados + superblock->bitmapb_offset;

	 off_t bitmapi_offset = superblock->bitmapi_offset;
	 lseek(file, bitmapi_offset, SEEK_SET);

	 if(BLOCK_SIZE < sizeof(bitmapi->bitmap)){
	 		 nBloquesReservados = sizeof(bitmapi->bitmap)/BLOCK_SIZE;
	 }

	 bitmapi = mmap(NULL,BLOCK_SIZE*nBloquesReservados,PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->bitmapi_offset);
	 printf("%d\n", sizeof(bitmapi->bitmap));

	printf("Todo guardado\n");
	//msync(superblock, BLOCK_SIZE, MS_SYNC);
	//munmap(superblock, BLOCK_SIZE);
	close(file);

	file = open("filesystem.txt", O_CREAT | O_RDWR, 0664);
	//superblock = mmap(NULL,BLOCK_SIZE,PROT_WRITE|PROT_READ,MAP_SHARED,file, 0);
	 lseek(file, 0, SEEK_SET);
	printf("%lu\n,%lu\n,%lu\n,%lu\n, %lu\n, %lu\n",superblock->magic_number, superblock->num_blocks, superblock->num_inodes, superblock->block_size, superblock->bitmapb_offset, superblock->bitmapi_offset);
	//munmap(superblock, BLOCK_SIZE);
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
