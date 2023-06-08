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

/* Creamos fichero en directorio "directory" con inodo "i_directory" */
int touch (char *name, char *path, filesystem_t *private_data) {

	int res = 0;
	struct inode_fs *i_directory;
	struct inode_fs *existente;
	struct inode_fs *inode;
	//struct directory_entry *entry;

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
		// Si no existe el inodo, lo creamos
		if (existente == NULL) {
			inode = create_inode('f', private_data);

			// Añadimos la nueva entrada al padre
			update_entry(name, inode, i_directory, private_data);

			// Incrementamos contador de enlaces
			inode->i_links++;
			i_directory->i_links++;

		} else {
			// printf("Ya existe el fichero o directorio\n");
			res = -1;
		}

	}
	
	return res;
}

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
//		printf("El fichero o directorio no existe");
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

/* Limpia un inodo */
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

// Renombramos un fichero (o directorio) a partir de su nombre (name) en su directorio (directory)
// Sólo cambiamos el nombre del registro de directorio (NO BORRAMOS ENTRY Y CREAMOS NUEVA, QUE SERÍA LO SUYO)

void rename_file(char *name, char *new_name, struct inode_fs *directory, struct block_bitmap_fs* bitmapb) {
	// Comprobamos si existe el fichero o directorio
	int i = 0;
	int encontrado = 0;
	int offset = 0;   // offset para desplazarnos por los bloques
	struct inode_fs *inodo = inode_search(name, directory, bitmapb);

	if (inodo == NULL) {
		printf("No existe el fichero\n");
		return;
	}

	// Buscamos la entrada de directorio
	struct directory_entry *entry = malloc(sizeof(struct directory_entry));

	while (i < N_DIRECTOS && (directory -> i_directos[i] != -1) && !encontrado) {
		// Recorremos el bloque (32 entradas) de cada puntero directo del inodo
		memcpy(entry, bitmapb -> map[directory -> i_directos[i]] + offset, sizeof(struct directory_entry));

		for (int j = 1; j < 32 && (entry -> inode != NULL) && !encontrado; j++) {
			if (strcmp(entry -> name, name) == 0) {
				// Hemos encontrado la entrada, cambiamos su nombre
				strcpy(entry -> name, new_name);
				memcpy(bitmapb -> map[directory -> i_directos[i]] + offset, entry, sizeof(struct directory_entry));
				encontrado = 1;
			} else {
				offset = sizeof(struct directory_entry) * j;
				memcpy(entry, bitmapb -> map[directory -> i_directos[i]] + offset, sizeof(struct directory_entry));
			}

			// MEJORA: en vez de cambiar el nombre a la entrada, eliminarla (setear a 0) y añadir nueva con new_name al mismo inodo
		}
		i++;
	}

	free(entry);
}

*/





