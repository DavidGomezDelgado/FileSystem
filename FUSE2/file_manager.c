#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include "filesystem.h"

/* Añade en el inodo padre su nuevo inodo hijo */
void update_entry (char *name, struct inode_fs *hijo, struct inode_fs *padre, filesystem_t *private_data) {

	// Accedemos al inodo padre y buscamos puntero libre
	struct directory_entry *entry;
	int i, j, encontrado = 0;

	// Recorremos punteros directos del padre
	//Nos saltamos . y .. ya que puede que algunos sea el inodo 0 que es el raiz
	entry = (struct directory_entry *) private_data->block[padre->i_directos[0] - private_data->superblock->reserved_block];
	for (j = 2; j < max_entries && !encontrado; j++) {
		// Si encontramos entrada libre, la actualizamos
		if(entry[j].inode == 0) {
			strcpy(entry[j].name, name);
			entry[j].inode = hijo->i_num;
			encontrado = 1;
		}
	}
	for (i = 1; i < N_DIRECTOS && (padre->i_directos[i] != 0) && !encontrado; i++) {
		entry = (struct directory_entry *) private_data->block[padre->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos las entradas
		for (j = 0; j < max_entries && !encontrado; j++) {

			// Si encontramos entrada libre, la actualizamos
			if(entry[j].inode == 0) {
				strcpy(entry[j].name, name);
				entry[j].inode = hijo->i_num;
				encontrado = 1;
			}
		}

	}

}

/* Creación de un fichero a partir del path del directorio que lo contiene */
int touch (char *name, char *path, filesystem_t *private_data) {

	int res = 0;
	struct inode_fs *i_directory;
	struct inode_fs *existente;
	struct inode_fs *inode;

	if (strncmp(name, ".", 1) == 0){
		printf("El nombre no puede empezar por .\n");
	} else {
		if(strcmp(path, "/") == 0){
			i_directory = &(private_data->inode[0]);
		}else{
			i_directory = inode_search_path(path, private_data);
		}
			existente = inode_search(name, i_directory, private_data);
		// Comprobamos si ya existe el inodo en el directorio
		if (existente == NULL) {
			inode = create_inode('f', private_data);

			// Añadimos la nueva entrada al padre
			update_entry(name, inode, i_directory, private_data);

			// Incrementamos contador de enlaces
			inode->i_links++;
			i_directory->i_links++;

		} else {
			res = -1;
		}

	}
	
	return res;
}

/* Renombre de un fichero o directorio a partir de un path */
int rename_file (char *path, char *new_name, filesystem_t *private_data) {

	int res = 0;
	struct inode_fs *i_directory;
	struct inode_fs *inode;
	struct inode_fs *inode_dst;   // para comprobar si ya existe alguien con el nombre new_name
	struct directory_entry *entry;
	int i, j, encontrado = 0;
	char path_aux[70], dir[70], base[70];

	strcpy(path_aux, path);

	// Obtenemos nombre del fichero o directorio y path hasta el directorio padre
	strcpy(base, basename(path_aux));  // Nos sirve para buscarlo en el inodo padre
	strcpy(dir, dirname(path_aux));

	inode = inode_search_path(path, private_data);

	// Obtenemos inodo del padre
	if(strcmp(path, "/") == 0){
		i_directory = &(private_data->inode[0]);
	}else{
		i_directory = inode_search_path(dir, private_data);
	}

	if (inode == NULL) {
		return -1;
	} else {

		inode_dst = inode_search(new_name, i_directory, private_data);

		// Comprobamos si existe algún archivo con el nombre new_name (sólo si es fichero)
		if (inode_dst != NULL && inode_dst->i_type == 'f') {
			// Existe y podemos reemplazarlo
			printf("Existe entrada con ese nombre... Borramos la existente\n");

			// Eliminamos la entrada del directorio actual a partir de su nombre
			remove_dentry(new_name, i_directory, private_data);

			// Eliminaos el inodo correspondiente
			clean_inode(inode_dst, private_data);
			remove_inode(inode_dst, private_data);

		}

		for (i = 0; i < N_DIRECTOS && (i_directory->i_directos[i] != 0) && !encontrado; i++) {
			entry = (struct directory_entry *) private_data->block[i_directory->i_directos[i] - private_data->superblock->reserved_block];

			for (j = 0; j < max_entries && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode >= 0) && !encontrado; j++) {
				// Comprobamos el nombre
				if (strcmp(entry[j].name, base) == 0) {
					strcpy(entry[j].name, new_name);
					encontrado = 1;
				}
			}
		}

	}

	return res;

}

/* Limpieza de un inodo */
void clean_inode (struct inode_fs *inodo, filesystem_t *private_data) {

	int byte, bit, i;
	inodo->i_tam = 0;

	// Recorremos los punteros directos
	for (i = 0; i < N_DIRECTOS && inodo->i_directos[i] != 0; i++) {
		// Seteamos a NULL el bloque
		byte = inodo->i_directos[i] / 8;
		bit = 7 - (inodo->i_directos[i] % 8);
		private_data->block_bitmap.array[byte] = private_data->block_bitmap.array[byte] & ~ (1 << bit);
		memset(private_data->block[inodo->i_directos[i] - private_data->superblock->reserved_block], 0, BLOCK_SIZE);
		// Ahora no apunta a nada
		inodo->i_directos[i] = 0;
	}

}
