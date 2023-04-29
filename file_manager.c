#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

void touch (char *nombre, char type, char *directory, struct inode_fs *inode, struct inode_bitmap_fs *bitmap, struct block_bitmap_fs *bitmapb){
	struct inode_fs *directorio = malloc(sizeof(struct inode_fs));
	directorio = inode_search(directory,inode);
	struct directory_entry *dir_contenido = malloc(sizeof(struct directory_entry));;
	int i, j, encontrado = 0;
	struct inode_fs *existente = malloc(sizeof(struct inode_fs));
	existente = inode_search(nombre, directorio);
	if(existente == NULL){
		if(directorio != NULL){
			struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
			inodo = create_inode(type,nombre, bitmap, bitmapb);
			for(i = 0; i < N_DIRECTOS && !encontrado; i++){
				for(j = 0; j < 31 && !encontrado; j++){
					memcpy(dir_contenido, directorio->i_directos[i] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
					if(dir_contenido == NULL){
						strcpy(dir_contenido->name, nombre);
						dir_contenido->inode = inodo;
						encontrado = 1;
					}
				}
			}
			//vamos a mirar los punteros indirectos
			if(!encontrado){
				//TODO
			}
			free(inodo);
		}
	}else{
		printf("Ya existe un fichero o directorio con ese nombre");
	}
	free(directorio);
	free(dir_contenido);
	free(existente);
	return;
}


void file_edit(char *contenido, char *nombre, /*char option,*/ struct inode_fs directory, struct block_bitmap_fs *bitmapb){
	struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
	struct directory_entry *file_contain = malloc(sizeof(struct directory_entry));
	inodo = search_inode(nombre, directory);
	int i, j;
	if(inodo == NULL){
		printf("No existe el fichero\n");
		return;
	}
	//borramos contenido
	for(int i = 0; i < N_DIRECTOS && inodo->i_directos[i] != NULL; i++){
		for(int j = 0; j < 32 && inodo->i_directos[i] != NULL; j++){
			//TODO
			//estructura del contenido del fichero, no es directory_entry hay que crearla
			memcpy(file_contain, inodo->i_directos[i] + sizeof(struct directory_entry)*j, sizeof(struct directory_entry));
			if(file_contain == NULL){
				break;
			}
			//setear a NULL
		}
	}
	//escribir el contenido

	return;
}
