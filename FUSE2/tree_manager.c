#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/* Buscamos un inodo a partir de su nombre y el inodo del directorio que lo contiene */
struct inode_fs *inode_search (char *name, struct inode_fs *directory, filesystem_t *private_data) {

	struct inode_fs *inode = NULL;
	struct directory_entry *entry;
	int encontrado = 0;
	int i, j;

	// Recorremos punteros directos del directorio
	for (i = 0; i < N_DIRECTOS && (directory->i_directos[i] != 0) && !encontrado; i++) {

		// Obtenemos entradas del bloque apuntado por el puntero directo
		entry = (struct directory_entry *) private_data->block[directory->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos entradas
		for (j = 0; j < 128 && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode >= 0) && !encontrado; j++) {
			if(strcmp(entry[j].name, name) == 0) {
				inode = &private_data->inode[entry[j].inode];
				encontrado = 1;
			}
		}
	}

	return inode;

}

/* Obtención del inodo de un fichero o directorio direccionado por un path */
struct inode_fs *inode_search_path(char *path, filesystem_t *private_data){
	char* nombre_dir;
	char camino[1000];
	if(path == NULL){
		printf("No ha introducido ningún path");
		return NULL;
	}
	strcpy(camino, path);
	struct inode_fs *inodo = &(private_data -> inode[0]);
	nombre_dir = strtok(camino, "/");
	if(nombre_dir != NULL){
		while(nombre_dir != NULL && inodo != NULL){
			inodo = inode_search(nombre_dir, &(private_data->inode[inodo->i_num]), private_data);
			nombre_dir = strtok(NULL, "/");
		}
	}
	return inodo;
}
