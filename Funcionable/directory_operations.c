#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/* Muestra el contenido del directorio "nombre" con inodo "directory" */
void read_directory (/*char *name,*/ char *path, /*struct inode_fs *i_directory,*/ filesystem_t *private_data) {

	int i = 0;
	int j, encontrado = 0;
	struct directory_entry *entry;
	struct inode_fs *i_directory;

	if(strcmp(path, "/") == 0){
		i_directory = &(private_data->inode[0]);
	}else{
		i_directory = inode_search_path(path, private_data);
	}

	// Comprobamos si existe el inodo y si es un directorio
	if (i_directory == NULL || i_directory->i_type != 'd') {
		printf("El directorio no existe o no es un directorio\n");
	} else {

		// Recorremos punteros directos del directorio
		while (i < N_DIRECTOS && (i_directory->i_directos[i] != 0)) {

			// Obtenemos entradas del bloque apuntado por el puntero directo
			entry = (struct directory_entry *) private_data->block[i_directory->i_directos[i] - private_data->superblock->reserved_block];
			printf("Contenido del directorio:\n");

			// Recorremos entradas
			for (j = 0; j < 128 && (entry[j].inode != NULL) && !encontrado; j++) {
				printf(" tipo: %c  permisos: %d  id: %d  tam: %d  links: %d  nombre: %s\n", entry[j].inode->i_type, entry[j].inode->i_permisos, entry[j].inode->i_num, entry[j].inode->i_tam, entry[j].inode->i_links, entry[j].name);
			}

			i++;

		}

	}

}


/*
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
*/
