#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

// FUNCIÓN ENTRY_SEARCH QUE DEVUELVA ENTRADA DE UN INODO

/* Creamos directorio en directorio "directory" con inodo "i_directory" */
void make_dir (char *name, char *path, filesystem_t *private_data) {

	struct inode_fs *i_directory;
	struct inode_fs *existente;
	struct inode_fs *inode;
	struct directory_entry *entry;
	int i, encontrado = 0;

	if (strncmp(name, ".", 1) == 0){
		printf("El nombre no puede empezar por .\n");
	} else {
		if(strcmp(path, "/") == 0){
			i_directory = &(private_data->inode[0]);
		}else{
			i_directory = inode_search_path(path, private_data);
		}
		// Comprobamos si ya existe el inodo en el directorio
		existente = inode_search(name, i_directory, private_data);

		// Si no existe el inodo, lo creamos
		if (existente == NULL) {
			inode = create_inode('d', private_data);

			// Añadimos la nueva entrada al padre
			update_entry(name, inode, i_directory, private_data);

			// Incrementamos contador de enlaces
			inode->i_links++;
			i_directory->i_links++;

			// Modificamos entrada .. para que apunte a su padre
			entry = (struct directory_entry *) private_data->block[inode->i_directos[0] - private_data->superblock->reserved_block];
			for (i = 0; i < 128 && (entry[i].inode != NULL) && !encontrado; i++) {
				if (strcmp(entry[i].name, "..") == 0) {
					entry[i].inode = i_directory;
					encontrado = 1;
				}
			}

		} else {
			printf("Ya existe el fichero o directorio\n");
		}

	}
}
