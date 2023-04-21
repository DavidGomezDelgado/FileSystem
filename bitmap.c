#include <stdio.h>

int free_inode(struct inode_bitmap *inode_bitmap){
	int byte = 0;
	// Encontrar el primer byte que no es 0xFF(el máximo)
	while(byte < NUM_INODES/8 && ((*inode_bitmap).bitmap[byte]) == 0xFF){
		byte++;
	}
	
	// Ahora buscamos el primer bit igual a 0
	int bit = 7;
	
	unsigned char aux = (*inode_bitmap).bitmap[byte];
	while(bit >= 0 && (((aux >>bit) & 1) != 0){
		bit--;
	}
	
	// Ponemos bit a 1
	(*inode_bitmap).bitmap[byte] |= (1 << bit)
}
