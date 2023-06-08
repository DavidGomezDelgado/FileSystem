#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "filesystem.h"

// FUNCIÓN ENTRY_SEARCH QUE DEVUELVA ENTRADA DE UN INODO

/* Creamos directorio a partir de un path */
int make_dir (char *name, char *path, filesystem_t *private_data) {

	struct inode_fs *i_directory;
	struct inode_fs *existente;
	struct inode_fs *inode;
	struct directory_entry *entry;
	int res= 0;

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
			entry[1].inode = i_directory->i_num;

		} else {
			//printf("Ya existe el fichero o directorio\n");
			res = -1;
		}

	}

	return res;

}

int rm_dir (char *name, char *path, filesystem_t *private_data) {

	int res = 0, vacio = 0;
	struct inode_fs *i_directory;
	struct inode_fs *existente;
	struct directory_entry *entry;

	// Buscamos inodo del padre
	if(strcmp(path, "/") == 0){
		i_directory = &(private_data->inode[0]);
	}else{
		i_directory = inode_search_path(path, private_data);
	}

	// Comprobamos si existe el inodo en el directorio
	existente = inode_search(name, i_directory, private_data);

	if (existente != NULL) {
		if (existente->i_type == 'd') {
			// Comprobamos si está vacío
			entry = (struct directory_entry *) private_data->block[existente->i_directos[0] - private_data->superblock->reserved_block];

			if (entry[2].inode == 0) {
				// Está vacío
				printf("%s está vacío\n", name);
				vacio = 1;
			}
		}

		if (vacio) {
			// Podemos eliminar su entrada del padre
			printf("Borramos entrada\n");
			remove_dentry(name, i_directory, private_data);

			// Eliminaos el inodo correspondiente
			clean_inode(existente, private_data);
			remove_inode(existente,  private_data);
		} else {
			printf("El directorio no está vacío\n");
			return 1;  // si no está vacío
		}
	} else {
		return -1;  // si no existe
	}

	return res;

}
