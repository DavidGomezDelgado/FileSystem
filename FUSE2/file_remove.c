#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include "filesystem.h"

/* Eliminación de inodo tanto en inode_bitmap como el array de inodos */
void remove_inode (struct inode_fs *inode, filesystem_t *private_data) {

	int byte = inode->i_num / 8;  // Obtenemos byte del bitmap
	int bit = 7 - (inode->i_num % 8);
	private_data->inode_bitmap.array[byte] = private_data->inode_bitmap.array[byte] &~ (1 << bit);

	//inode = NULL;
	memset(&private_data->inode[inode->i_num], 0, sizeof(struct inode_fs));
	private_data->superblock->inodes_ocupados--;

}

/* Obtención de la última entrada de un directorio */
struct directory_entry *search_last_entry (struct inode_fs *i_directorio, filesystem_t *private_data) {

	int i, j, byte, bit;
	struct directory_entry *entry;
	
	entry = (struct directory_entry *) private_data->block[i_directorio->i_directos[0] - private_data->superblock->reserved_block];
	// Recorremos las entradas, nos saltamos . .. 
	for (j = 2; j < max_entries-1 && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode > 0); j++) {

		// Comprobamos la siguiente entrada
		if (entry[j+1].inode == 0) {
			return &entry[j];

		}

	}
	// Recorremos punteros directos excepto el primero que ya ha sido comprobado
	for (i = 2; i < N_DIRECTOS && (i_directorio->i_directos[i] != 0); i++) {
		// Obtenemos entradas
		entry = (struct directory_entry *) private_data->block[i_directorio->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos las entradas
		for (j = 0; j < max_entries && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode > 0); j++) {
			//printf("search_last_entry -> entrada %d: %s  entrada %d: %s\n", j, entry[j].name, j+1, entry[j+1].name);

			// Comprobamos la siguiente entrada
			if (entry[j+1].inode == 0) {
				// Hemos llegado a la última

				// Si esta entrada es la 1ª de un bloque seteamos a 0 en block_bitmap
				if (j == 0) {
					byte = i_directorio->i_directos[i] / 8;
					bit = 7 - (i_directorio->i_directos[i] % 8);
					private_data->block_bitmap.array[byte] = private_data->block_bitmap.array[byte] & ~(1 << bit);

				}
				//printf("search_last_entry -> entrada %d: %s\n", j, entry[j].name);
				return &entry[j];

			}

		}
	}

	return &entry[j];

}

/* Eliminación de una entrada de directorio */
void remove_dentry (char *nombre, struct inode_fs *i_directorio, filesystem_t *private_data) {

	unsigned long i, j;
	int encontrado = 0;
	struct directory_entry *entry;
	struct directory_entry *last;

	// Obtenemos entradas
	entry = (struct directory_entry *) private_data->block[i_directorio->i_directos[0] - private_data->superblock->reserved_block];
	// Recorremos las entradas buscando la que queremos eliminar
	for (j = 2; j < max_entries && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode > 0) && !encontrado; j++) {
		//printf("remove_dentry -> entrada: %s\n", entry[j].name);

		if (strcmp(entry[j].name, nombre) == 0) {
			// Obtenemos la última entrada para moverla a la que queremos eliminar
			last = search_last_entry(i_directorio, private_data);

			if (strcmp(entry[j].name, last->name) != 0) {
				//printf("remove_dentry -> entrada a borrar: %s  entrada last: %s\n", entry[j].name, last->name);

				// Copiamos última entrada en la borrada y liberamos last
				memset(&entry[j], 0, sizeof(struct directory_entry));
				//memcpy(&entry[j] ,last, sizeof(struct directory_entry));
				strcpy(entry[j].name, last->name);
				entry[j].inode = last->inode;
				memset(last, 0, sizeof(struct directory_entry));

			} else {
				// Si la última coincide con la que queremos eliminar, sólo la liberamos
				memset(last, 0, sizeof(struct directory_entry));
			}

			encontrado = 1;
		}
	}

	// Recorremos punteros directos del inodo padre
	for (i = 1; i < N_DIRECTOS && (i_directorio->i_directos[i] != 0) && !encontrado; i++) {

		// Obtenemos entradas
		entry = (struct directory_entry *) private_data->block[i_directorio->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos las entradas buscando la que queremos eliminar
		for (j = 0; j < max_entries && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode > 0) && !encontrado; j++) {
			//printf("remove_dentry -> entrada: %s\n", entry[j].name);

			if (strcmp(entry[j].name, nombre) == 0) {
				// Obtenemos la última entrada para moverla a la que queremos eliminar
				last = search_last_entry(i_directorio, private_data);

				if (strcmp(entry[j].name, last->name) != 0) {
					//printf("remove_dentry -> entrada a borrar: %s  entrada last: %s\n", entry[j].name, last->name);

					// Copiamos última entrada en la borrada y liberamos last
					strcpy(entry[j].name, last->name);
					entry[j].inode = last->inode;
					memset(last, 0, sizeof(struct directory_entry));

				} else {
					// Si la última coincide con la que queremos eliminar, sólo la liberamos
					memset(last, 0, sizeof(struct directory_entry));
				}

				encontrado = 1;
			}
		}
	}

}

/* Eliminación de un fichero a partir de un path */
int rm (char *path, filesystem_t *private_data) {

	int res = 0;
	struct inode_fs *i_directory;
	struct inode_fs *inode;
	char path_aux[70], base[70], dir[70];

	// Obtenemos inodo del fichero o directorio
	inode = inode_search_path(path, private_data);

	strcpy(path_aux, path);

	// Obtenemos nombre del fichero o directorio y path hasta el directorio padre
	strcpy(base, basename(path_aux));  // Nos sirve para buscarlo en el inodo padre
	strcpy(dir, dirname(path_aux));

	// Obtenemos inodo del padre
	if(strcmp(path, "/") == 0){
		i_directory = &(private_data->inode[0]);
	}else{
		i_directory = inode_search_path(dir, private_data);
	}

	if (inode == NULL) {
		return -1;
	} else if (inode->i_type != 'f') {
		return 1;
	} else {
		// Eliminamos la entrada del directorio actual a partir de su nombre
		remove_dentry(base, i_directory, private_data);

		// Eliminaos el inodo correspondiente
		clean_inode(inode, private_data);
		remove_inode(inode,  private_data);

	}
	
	return res;

}
