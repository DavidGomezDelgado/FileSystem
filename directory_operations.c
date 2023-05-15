#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

char *read_directory(char *nombre,struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
	int i, j;
	char *contenido;
	struct inode_fs *inodo;
	if(strcmp(nombre, "/")){
		inodo = inode_search(nombre, directory, bitmapb);
	}else{
		inodo = directory;
	}

	if(inodo == NULL){
		printf("No existe el directorio %s en este directorio\n", nombre);
	}else{
		struct directory_entry *aux = malloc(sizeof(struct directory_entry));
		char* buffer;
		for(i = 0; i < N_DIRECTOS && inodo->i_directos[i] != -1; i++){
			printf("Entradas puntero %d de %s:\n", i, nombre);
			memcpy(aux, bitmapb->map[inodo->i_directos[i]], sizeof(struct directory_entry));
			for(j = 0; j < 32 && aux->inode != NULL; j++){
				printf("Entrada %s %d: Nombre: %s, %d\n",nombre,i, aux->name, aux->inode->i_num);
				fflush(stdout);
				if(j+1 < 32)
				memcpy(aux, bitmapb->map[inodo->i_directos[i]]+sizeof(struct directory_entry)*(j+1), sizeof(struct directory_entry));
			}
		}
		free(aux);
	}

	return NULL;
}


