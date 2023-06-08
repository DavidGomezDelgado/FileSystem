#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/* Muestra el contenido de un directorio direccionado por un path */
void read_directory (char *path, filesystem_t *private_data) {

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
		//Reccorremos el primer puntero directo (condicion distinta)
		entry = (struct directory_entry *) private_data->block[i_directory->i_directos[i] - private_data->superblock->reserved_block];
		printf("Contenido del directorio:\n");

		// Recorremos entradas
		printf(" tipo: %c  permisos: %d  id: %ld  tam: %d  links: %d  nombre: %s\n", private_data->inode[entry[0].inode].i_type, private_data->inode[entry[0].inode].i_permisos, private_data->inode[entry[0].inode].i_num, private_data->inode[entry[0].inode].i_tam, private_data->inode[entry[0].inode].i_links, entry[0].name);
		printf(" tipo: %c  permisos: %d  id: %ld  tam: %d  links: %d  nombre: %s\n", private_data->inode[entry[1].inode].i_type, private_data->inode[entry[1].inode].i_permisos, private_data->inode[entry[1].inode].i_num, private_data->inode[entry[1].inode].i_tam, private_data->inode[entry[1].inode].i_links, entry[1].name);
		for (j = 2; j < 128 && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode > 0) && !encontrado; j++) {
			printf(" tipo: %c  permisos: %d  id: %ld  tam: %d  links: %d  nombre: %s\n", private_data->inode[entry[j].inode].i_type, private_data->inode[entry[j].inode].i_permisos, private_data->inode[entry[j].inode].i_num, private_data->inode[entry[j].inode].i_tam, private_data->inode[entry[j].inode].i_links, entry[j].name);
		}

		i++;
		// Recorremos punteros directos del directorio
		while (i < N_DIRECTOS && (i_directory->i_directos[i] != 0)) {

			// Obtenemos entradas del bloque apuntado por el puntero directo
			entry = (struct directory_entry *) private_data->block[i_directory->i_directos[i] - private_data->superblock->reserved_block];
			printf("Contenido del directorio:\n");

			// Recorremos entradas
			for (j = 0; j < 128 && (entry[j].inode <= private_data->inode_bitmap.size && entry[j].inode > 0) && !encontrado; j++) {
				printf(" tipo: %c  permisos: %d  id: %ld  tam: %d  links: %d  nombre: %s\n", private_data->inode[entry[j].inode].i_type, private_data->inode[entry[j].inode].i_permisos, private_data->inode[entry[j].inode].i_num, private_data->inode[entry[j].inode].i_tam, private_data->inode[entry[j].inode].i_links, entry[j].name);
			}

			i++;

		}

	}

}
