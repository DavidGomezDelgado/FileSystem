#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../filesystem.h"

#define NUM_BLOCKS 1000
#define NUM_INODES 1000

#define BLOCK_SIZE 1024

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
}
