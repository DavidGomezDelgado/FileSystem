#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <string.h>
//#include <errno.h>
#include "filesystem.h"

/* Ontención del primer índice libre */
unsigned long free_bit(struct bitmap_t *bitmap){
	int byte = 0;
	unsigned long index;

	// Encontrar el primer byte que no es 0xFF(el máximo)
	while(byte < bitmap->size && ((bitmap->array[byte]) == 0xFF)){
		byte++;
	}

	if(byte == bitmap->size){
		return (bitmap->size * 8) + 1;
	}
	// Ahora buscamos el primer bit igual a 0
	int bit = 7;

	unsigned char aux = bitmap->array[byte];
	while(bit >= 0 && (((aux >> bit) & 1) != 0)){
		bit--;
	}

	// Ponemos bit a 1
	bitmap->array[byte] |= (1 << bit);

	index = (byte * 8) + (7 - bit);

	return index;
}

