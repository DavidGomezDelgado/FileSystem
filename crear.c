#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filesystem.h"

/*inode_bitmap create_bitmap (void){
	struct inode_bitmap bitmap = malloc (sizeof(struct inode_bitmap));
	return bitmap;
}*/

inode create_Inode (char[24] name, char type, inode_bitmap *i_bitmap){
	//crear inodo
	struct inode inodo = malloc (sizeof(struct inode));
	int directos[N_DIRECTOS];
	int indirecto_simple[N_SIMPLES];
	inodo->i_num = 0;
	strcpy(inodo->i_name, name);
	inodo->i_type = type;
	inodo->i_tam = 0;
	inodo->i_directos = directos;
	inodo->i_simple_ind = indirecto_simple;
	
	//buscar byte libre
	int libre = 0;
	while(libre != (NUM_INODES/8) && bitmap[libre] != 0xFF){
		libre++;
	}
	//buscar bit más significativo libre en el byte encontrado 
	if(libre == (NUM_INODES/8)){
		return -1;
	}else{
		int bitl = 7;
		unsigned char aux = (*i_bitmap).bitmap[libre];
		while(bit >= 0 && ((aux >> bitl) & 1) != 0){
			bitl--;
		}
		(*i_bitmap).bitmap[libre] |= (1 << bitl);
		
	}
	
	return inode;
}

/*block create_block (int size){
	struct block bloque = malloc (sizeof(block));
	
	bloque->(*memory_address) = ;
	bloque->block_size = size;
	bloque->next = NULL;
	
	return bloque;
}*/

struct superblock create_sblock (long n_free_blocks, long inode_list_sz){
	struct superblock *nuevo_sblock = malloc(sizeof(struct superblock));
	
	nuevo_sblock->free_blocks = n_free_blocks;
	nuevo_sblock->free_blocks_list = malloc (sizeof(struct block) * n_free_blocks);
	
	nuevo_sblock->inode_list_size = inode_list_sz;
	nuevo_sblock->free_inodes_list = malloc(sizeof(struct inode) * inode_list_sz);
	
	nuevo_sblock->MODIFIED = "N";
	return nuevo_sblock;
}
