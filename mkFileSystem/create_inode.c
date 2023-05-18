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
	unsigned int bloque;
	memset(directos, -1, sizeof(int)*N_DIRECTOS);
	inodo -> i_num = free_inode(i_bitmap);
	inodo -> i_type = type;
	inodo->i_permisos = 666;
	memcpy(inodo -> i_directos, directos, sizeof(int)*N_DIRECTOS);
	//memcpy(inodo -> i_simple_ind, indirecto_simple, sizeof(int)*N_SIMPLES);

	//Completar para directorios
	if(inodo->i_type == 'd'){
		bloque = free_block(i_bitmapb);
		inodo->i_directos[0] = bloque;
		inodo -> i_tam = sizeof(struct directory_entry) * 2;
	}else {
		inodo -> i_tam = 0;
	}
	
	return inodo;
}

struct inode_fs *create_root(struct inode_bitmap_fs *i_bitmap, struct block_bitmap_fs *i_bitmapb){
	struct inode_fs *inodo = malloc (sizeof(struct inode_fs));
	int *directos = malloc(sizeof(int)*N_DIRECTOS);
	memset(directos, -1, sizeof(int)*N_DIRECTOS);
	unsigned int bloque;
	inodo -> i_num = free_inode(i_bitmap);
	inodo -> i_type = 'd';
	inodo -> i_tam = sizeof(struct directory_entry) * 2;
	inodo->i_permisos = 666;
	memcpy(inodo -> i_directos, directos, sizeof(int)*N_DIRECTOS);
	//memcpy(inodo -> i_simple_ind, indirecto_simple, sizeof(int)*N_SIMPLES);
	bloque = free_block(i_bitmapb);
	inodo->i_directos[0] = bloque;
	struct directory_entry *directory = malloc(sizeof(struct directory_entry));
	strcpy(directory->name, ".");
	directory->inode = inodo;
	//memcpy(i_bitmapb->map[bloque], directory, sizeof(struct directory_entry));
	strcpy(directory->name, "..");
	//memcpy(i_bitmapb->map[bloque]+sizeof(struct directory_entry), directory, sizeof(struct directory_entry));
	free(directos);
	return inodo;
}

