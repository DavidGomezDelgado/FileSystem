#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

int free_inode(struct inode_bitmap_fs *inode_bitmap){
	int byte = 0;
	int num_inode;
	
	// Encontrar el primer byte que no es 0xFF(el máximo)
	while(byte < NUM_INODES/8 && ((*inode_bitmap).bitmap[byte]) == 0xFF){
		byte++;
	}
	
	// Ahora buscamos el primer bit igual a 0
	int bit = 7;
	
	unsigned char aux = (*inode_bitmap).bitmap[byte];
	while(bit >= 0 && (((aux >>bit) & 1) != 0)){
		bit--;
	}
	
	// Ponemos bit a 1
	(*inode_bitmap).bitmap[byte] |= (1 << bit);
	
	num_inode = (byte * 8) + (7 - bit);
	
	return num_inode;
}

int free_block(struct block_bitmap_fs *block_bitmap){
	int byte = 0;
	unsigned int block;
	
	// Encontrar el primer byte que no es 0xFF(el máximo)
	while(byte < NUM_BLOCKS/8 && ((*block_bitmap).bitmap[byte]) == 0xFF){
		byte++;
	}

	// Ahora buscamos el primer bit igual a 0
	int bit = 7;
	
	unsigned char aux = (*block_bitmap).bitmap[byte];
	while(bit >= 0 && (((aux >>bit) & 1) != 0)){
		bit--;
	}
	
	// Ponemos bit a 1
	(*block_bitmap).bitmap[byte] |= (1 << bit);
	
	block = (byte * 8) + (7 - bit);
	

	return block;
}