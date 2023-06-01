#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/* Añade en el inodo padre su nuevo inodo hijo */
void update_entry (char *name, struct inode_fs *hijo, struct inode_fs *padre, filesystem_t *private_data) {

	// Accedemos al inodo padre y buscamos puntero libre
	struct directory_entry *entry;
	int i, j, encontrado = 0;

	// Recorremos punteros directos del padre
	for (i = 0; i < N_DIRECTOS && (padre->i_directos[i] != 0) && !encontrado; i++) {
		entry = (struct directory_entry *) private_data->block[padre->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos las 128 entradas
		for (j = 0; j < 128 && !encontrado; j++) {

			// Si encontramos entrada libre, la actualizamos
			if(entry[j].inode == NULL) {
				strcpy(entry[j].name, name);
				entry[j].inode = hijo;
				encontrado = 1;
			}
		}

	}

}

/* Creamos fichero en directorio "directory" con inodo "i_directory" */
void touch (char *name, char type, char *directory, struct inode_fs *i_directory, filesystem_t *private_data) {

	struct inode_fs *existente;
	struct inode_fs *inode;
	struct directory_entry *entry;
	int i, encontrado = 0;

	if (strncmp(name, ".", 1) == 0){
		printf("El nombre no puede empezar por .\n");
	} else {

		// Comprobamos si ya existe el inodo en el directorio
		existente = inode_search(name, i_directory, private_data);

		// Si no existe el inodo, lo creamos
		if (existente == NULL) {
			inode = create_inode(type, private_data);

			// Añadimos la nueva entrada al padre
			update_entry(name, inode, i_directory, private_data);

			// Incrementamos contador de enlaces
			inode->i_links++;
			i_directory->i_links++;

			if (type == 'd') {
				// Modificamos entrada .. para que apunte a su padre
				entry = (struct directory_entry *) private_data->block[inode->i_directos[0] - private_data->superblock->reserved_block];
				for (i = 0; i < 128 && (entry[i].inode != NULL) && !encontrado; i++) {
					if (strcmp(entry[i].name, "..") == 0) {
						entry[i].inode = i_directory;
						encontrado = 1;
					}
				}
			}

		} else {
			printf("Ya existe el fichero o directorio\n");
		}

	}
}

/* Limpia un inodo */
void clean_inode(struct inode_fs *inodo, filesystem_t *private_data){


}


/*

// FUNCIÓN ANTIGUA (LA HACEMOS EN CREATE_INODE)

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

// FUNCIÓN ANTIGUA

// directory=nombre directorio donde se creará,  inode=inodo directorio donde se creará
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

// FUNCIÓN ANTIGUA

void clean_inode(struct inode_fs *inodo, struct block_bitmap_fs *bitmapb){
	int bit, byte;
	for(int i = 0; i < N_DIRECTOS && inodo->i_directos[i] != -1; i++){
			//TODO
			//setear a NULL
			byte = inodo->i_directos[i] / 8;
			bit = 7 - (inodo->i_directos[i] % 8);
			bitmapb->bitmap[byte] = bitmapb->bitmap[byte] & ~ (1 << bit);
			free(bitmapb->map[inodo->i_directos[i]]);
			bitmapb->map[inodo->i_directos[i]] = 0;
			inodo->i_directos[i] = -1;
	}
}

*/






