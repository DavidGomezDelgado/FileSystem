#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

void file_edit(char *contenido, char *nombre, struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
	char* cadena = malloc(BLOCK_SIZE);
	struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
	inodo = inode_search(nombre, directory, bitmapb);
	int i, j, buffer = strlen(contenido)+1, bloques;
	//char *posBlock;
	if(inodo == NULL){
		printf("No existe el fichero\n");
		return;
	}
	//borramos contenido
	clean_inode(inodo, bitmapb);

	//escribir el contenido
	inodo->i_tam = buffer;
	if(buffer < BLOCK_SIZE){
		for(j = 0; j< buffer; j++){
			cadena[j] = contenido[j];
		}
	}
	cadena[buffer] = '\o';

	if(buffer%BLOCK_SIZE == 0){
			bloques = (buffer)/BLOCK_SIZE;
	}else{
			bloques = (buffer)/BLOCK_SIZE+1;
	}

	//Reservamos el numero de bloques
	for(i = 0; i < bloques; i++){
		inodo->i_directos[i] = free_block(bitmapb);
	}

	i = 0;
	for(j = 0; j < bloques; j++){
		memcpy(bitmapb->map[inodo->i_directos[i]], cadena, BLOCK_SIZE);
		i++;
	}
	free(cadena);

	return;
}

char *read_file(char *nombre,struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
	struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
	int i;
	inodo = inode_search(nombre, directory,bitmapb);
	if(inodo == NULL){
		printf("El fichero no existe en el directorio");
	}else{
		char *contenidoTotal;
		contenidoTotal[0] = '\0';
		for(i = 0; i < N_DIRECTOS && inodo->i_directos[i] != -1; i++){
			strcat(contenidoTotal,bitmapb->map[inodo->i_directos[i]]);  // Creo que no estamos accedeindo a caracteres
		}
		printf("La cadena es: \n%s\n", contenidoTotal);
		return contenidoTotal;
	}
	free(inodo);
	return NULL;//DEVOLVER ERROR
}





















