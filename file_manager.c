#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

void es_directorio(struct inode_fs *i_hijo, struct inode_fs *i_padre, struct block_bitmap_fs *bitmapb){
	struct directory_entry *dir_contenido = malloc(sizeof(struct directory_entry));
	strcpy(dir_contenido->name, ".");
	dir_contenido->inode = i_hijo;
	memcpy(bitmapb->map[i_hijo->i_directos[0]], dir_contenido, sizeof(struct directory_entry));
	strcpy(dir_contenido->name, "..");
	dir_contenido->inode = i_padre;
	memcpy((bitmapb->map[i_hijo->i_directos[0]] + sizeof(struct directory_entry)), dir_contenido, sizeof(struct directory_entry));
	free(dir_contenido);

}
/* inode es el inodo del directorio raiz */
void touch (char *nombre, char type, char *directory, struct inode_fs *inode, struct inode_bitmap_fs *bitmap, struct block_bitmap_fs *bitmapb){
	if(strncmp(nombre, ".",1) == 0){
		printf("El  nombre no puede empezar por .\n");
	}else{
		struct inode_fs *directorio = malloc(sizeof(struct inode_fs));
		//Creo que podemos quitar la primera condición
		if(strcmp(directory, "/")){
			directorio = inode_search(directory,inode,bitmapb);
		}else{
			memcpy(directorio, inode, sizeof(struct inode_fs));
		}
			struct directory_entry *dir_contenido = malloc(sizeof(struct directory_entry));
			int i, j, encontrado = 0;
			struct inode_fs *existente = malloc(sizeof(struct inode_fs));
			if(directorio != NULL) existente = inode_search(nombre, directorio,bitmapb);
			else existente = NULL;
			//Buscamos si existe un inodo en un directorio concreto
			if(existente == NULL){
				struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
				inodo = create_inode(type,nombre, bitmap, bitmapb);
				//Miramos en todos sus punteros directos
				for(i = 0; i < N_DIRECTOS && !encontrado; i++){
					if(directorio->i_directos[i] != -1){
						for(j = 0; j < 31 && !encontrado; j++){
							memcpy(dir_contenido, bitmapb->map[directorio->i_directos[i]] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
							//Si llegamos a una entrada de bloque que no tenga información lo rellenamos con la información nueva
							if(dir_contenido->inode == NULL){
								strcpy(dir_contenido->name, nombre);
								dir_contenido->inode = inodo;
								memcpy((bitmapb->map[directorio->i_directos[i]] + sizeof(struct directory_entry)*j), dir_contenido, sizeof(struct directory_entry));
								if(type == 'd'){
									//Le decimos al nuevo inodo creado quién es su padre y quién es él
									es_directorio(inodo, inode, bitmapb);
								}
								inode->i_tam += sizeof(struct directory_entry);
								encontrado = 1;
							}
						}

					}else{
						directorio->i_directos[i] = free_block(bitmapb);
						strcpy(dir_contenido->name, nombre);
						dir_contenido->inode = inodo;
						bitmapb->map[directorio->i_directos[i]] = dir_contenido;
						if(type == 'd'){
							//Le decimos al nuevo inodo creado quién es su padre y quién es él
							es_directorio(inodo, inode, bitmapb);
						}
						encontrado = 1;
					}
				}
				free(dir_contenido);
				//vamos a mirar los punteros indirectos
				if(!encontrado){
					//TODO
				}
			}else{
				printf("Ya existe un fichero o directorio con ese nombre");
			}
			//free(directorio);
			//free(dir_contenido);
			free(existente);
	}

	return;
}

void file_edit(char *contenido, char *nombre, /*char option,*/ struct inode_fs directory, struct block_bitmap_fs *bitmapb){
	struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
	struct directory_entry *file_content = malloc(sizeof(struct directory_entry));
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
			memcpy(file_content, inodo->i_directos[i] + sizeof(char)*j, sizeof(char));
			if(file_content == NULL){
				break;
			}else{
				inodo->i_directos[i] + sizeof(char)*j = NULL;
			}
			//setear a NULL
		}
	}
	//escribir el contenido

	return;
}





































