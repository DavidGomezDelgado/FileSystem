#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

char *read_directory(char *nombre,struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
	int i, j;
	char *contenido;
	struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
	if(strcmp(nombre, "/")){
		inodo = inode_search(nombre, directory, bitmapb);
	}else{
		memcpy(inodo, directory, sizeof(struct inode_fs));
	}

	if(inodo == NULL){
		printf("No existe el directorio %s en este directorio\n", nombre);
	}else{
		struct directory_entry *entrada = malloc(sizeof(struct directory_entry));
		struct directory_entry *aux = malloc(sizeof(struct directory_entry));
		char* buffer;
		for(i = 0; i < N_DIRECTOS && inodo->i_directos[i] != -1; i++){
			memcpy(aux, bitmapb->map[inodo->i_directos[i]], sizeof(struct directory_entry));
			for(j = 0; j < 32 && aux != NULL; j++){
				memcpy(aux, bitmapb->map[inodo->i_directos[i]]+sizeof(struct directory_entry)*j, sizeof(struct directory_entry));
				memcpy(entrada, (bitmapb->map[inodo->i_directos[0]]+sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
				sprintf(buffer, "Entrada %s %d: Nombre: %s, %d",nombre,i, entrada->name, entrada->inode->i_num);
				printf("%s\n", buffer);
				fflush(stdout);
				strcat(contenido, buffer);
			}
		}
		printf("El contenido de %s es: %s", nombre, contenido);
	}
	free(inodo);
	return NULL;
}


