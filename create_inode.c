#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/*inode_bitmap create_bitmap (void){
	struct inode_bitmap bitmap = malloc (sizeof(struct inode_bitmap));
	return bitmap;
}*/


/* Creación de un inodo */
struct inode_fs *create_inode (char type, char *name, struct inode_bitmap_fs *i_bitmap, struct block_bitmap_fs *i_bitmapb){
	// Inicialización
	struct inode_fs *inodo = malloc (sizeof(struct inode_fs));   // malloc devuelve ptr
	int *directos[N_DIRECTOS];
	int bloque;
	memset(directos, -1, sizeof(int)*N_DIRECTOS);
	int *indirecto_simple[N_SIMPLES];
	memset(indirecto_simple,-1, sizeof(int)*N_SIMPLES);
	inodo -> i_num = free_inode(i_bitmap);
	strcpy(inodo -> i_name, name);
	inodo -> i_type = type;
	inodo -> i_tam = 0;
	memcpy(inodo -> i_directos, directos, sizeof(int)*N_DIRECTOS);
	if(inodo->i_type == 'd'){
		bloque = free_block(i_bitmapb);
		inodo->i_directos[0] = bloque;
	}
	memcpy(inodo -> i_simple_ind, indirecto_simple, sizeof(int)*N_SIMPLES);
	
	return inodo;
}

struct inode_fs *create_root(struct inode_bitmap_fs *i_bitmap, struct block_bitmap_fs *i_bitmapb){
	struct inode_fs *inodo = malloc (sizeof(struct inode_fs));
	int *directos = malloc(sizeof(int)*N_DIRECTOS);
	memset(directos, -1, sizeof(int)*N_DIRECTOS);
	int *indirecto_simple = malloc(sizeof(int)*N_SIMPLES);
	memset(indirecto_simple,-1, sizeof(int)*N_SIMPLES);
	int bloque;
	inodo -> i_num = free_inode(i_bitmap);
	strcpy(inodo -> i_name, "/");
	inodo -> i_type = 'd';
	inodo -> i_tam = 0;
	memcpy(inodo -> i_directos, directos, sizeof(int)*N_DIRECTOS);
	bloque = free_block(i_bitmapb);
	inodo->i_directos[0] = bloque;
	memcpy(inodo -> i_simple_ind, indirecto_simple, sizeof(int)*N_SIMPLES);
	free(directos);
	free(indirecto_simple);
	return inodo;
}

/*struct block create_block (int size){
	struct block bloque = malloc (sizeof(block));
	
	bloque->(*memory_address) = ;
	bloque->block_size = size;
	bloque->next = NULL;
	
	return bloque;
}*/

/* Creación del superbloque */
/*struct superblock *create_sblock (long n_free_blocks, long inode_list_sz){
	struct superblock *nuevo_sblock = malloc(sizeof(struct superblock));
	
	nuevo_sblock -> free_blocks = n_free_blocks;
	nuevo_sblock -> free_blocks_list = malloc(sizeof(struct block) * n_free_blocks);
	
	nuevo_sblock -> inode_list_size = inode_list_sz;
	nuevo_sblock -> free_inodes_list = malloc(sizeof(struct inode) * inode_list_sz);
	
	nuevo_sblock -> MODIFIED = 'N';
	
	return nuevo_sblock;
}*/
