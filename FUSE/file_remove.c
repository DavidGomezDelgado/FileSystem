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

}

struct directory_entry *search_last_entry (struct inode_fs *i_directorio, filesystem_t *private_data) {

	int i, j, byte, bit;
	struct directory_entry *entry;

	// Recorremos punteros directos menos el último
	for (i = 0; i < N_DIRECTOS && (i_directorio->i_directos[i] != 0); i++) {
		// Obtenemos entradas
		entry = (struct directory_entry *) private_data->block[i_directorio->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos las entradas
		for (j = 0; j < max_entries && (entry[j].inode != NULL); j++) {
			//printf("search_last_entry -> entrada %d: %s  entrada %d: %s\n", j, entry[j].name, j+1, entry[j+1].name);

			// Comprobamos la siguiente entrada
			if (entry[j+1].inode == NULL) {
				// Hemos llegado a la última
//				strcpy(entry[j].name, "");
//				entry[j].inode = NULL;

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

	// CAUTION !! POSIBLE FALLO DE ÚLIMO PUNTERO DIRECTO

	return &entry[j];

}

void remove_dentry (char *nombre, struct inode_fs *i_directorio, filesystem_t *private_data) {

	unsigned long i, j;
	int encontrado = 0;
	struct directory_entry *entry;
	struct directory_entry *last;

	// Recorremos punteros directos del inodo padre
	for (i = 0; i < N_DIRECTOS && (i_directorio->i_directos[i] != 0) && !encontrado; i++) {

		// Obtenemos entradas
		entry = (struct directory_entry *) private_data->block[i_directorio->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos las entradas buscando la que queremos eliminar
		for (j = 0; j < max_entries && (entry[j].inode != NULL) && !encontrado; j++) {
			//printf("remove_dentry -> entrada: %s\n", entry[j].name);

			if (strcmp(entry[j].name, nombre) == 0) {
				// Obtenemos la última entrada para moverla a la que queremos eliminar
				last = search_last_entry(i_directorio, private_data);

				if (strcmp(entry[j].name, last->name) != 0) {
					//printf("remove_dentry -> entrada a borrar: %s  entrada last: %s\n", entry[j].name, last->name);

					// Copiamos última entrada en la borrada y liberamos last
					memcpy(&entry[j] ,last, sizeof(struct directory_entry));
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
		// printf("No existe el fichero\n");
		return -1;
	} else if (inode->i_type != 'f') {
		// printf("No es un fichero\n");
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





/*
struct directory_entry *search_last_entry(struct inode_fs *i_directorio, struct block_bitmap_fs *bitmapb){
	int i, j;
	int byte;
	int bit;
	struct directory_entry *last = malloc(sizeof(struct directory_entry));
	struct directory_entry *aux;
	struct directory_entry *vacio = malloc(sizeof(struct directory_entry));
	for(i = 0; i < N_DIRECTOS-1 && i_directorio->i_directos[i] != -1; i++){
		for(j = 0; j<32 && (bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*j) != NULL; j++){
			if(j+1 < 32){
				aux=bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*(j+1);
				if(aux->inode == NULL){
					memcpy(last, (bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
					memcpy((bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*j),vacio,sizeof(struct directory_entry));
					if(j == 0 ){
						byte = i_directorio->i_directos[i] / 8;
						bit = 7 - (i_directorio->i_directos[i] % 8);
						bitmapb->bitmap[byte] = bitmapb->bitmap[byte] & ~ (1 << bit);
						free(bitmapb->map[i_directorio->i_directos[i]]);
						i_directorio->i_directos[i] = -1;
						}
						free(vacio);
						return last;
					}
			}
		}
	}
	for(j=0; j<31 && (bitmapb->map[i_directorio->i_directos[N_DIRECTOS-1]]+sizeof(struct directory_entry)*j) != NULL; j++){
		if(j+1 < 31){
			aux=bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*(j+1);
			if(aux->inode == NULL){
				memcpy(last, (bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
				memcpy((bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*j),vacio, sizeof(struct directory_entry));
				if(j == 0 ){
					byte = i_directorio->i_directos[i] / 8;
					bit = 7 - (i_directorio->i_directos[i] % 8);
					bitmapb->bitmap[byte] = bitmapb->bitmap[byte] & ~ (1 << bit);
					bitmapb->bitmap[i_directorio->i_directos[i]] = 0;
					free(bitmapb->map[i_directorio->i_directos[i]]);
					i_directorio->i_directos[i] = -1;
				}
				free(vacio);
				return last;
			}
		}
	}
	memcpy(last, (bitmapb->map[i_directorio->i_directos[N_DIRECTOS-1]] + sizeof(struct directory_entry)*31), sizeof(struct directory_entry));
	memcpy((bitmapb->map[i_directorio->i_directos[i]]+sizeof(struct directory_entry)*j),vacio,sizeof(struct directory_entry));
	if(j == 0 ){
		byte = i_directorio->i_directos[i] / 8;
		bit = 7 - (i_directorio->i_directos[i] % 8);
		bitmapb->bitmap[byte] = bitmapb->bitmap[byte] & ~ (1 << bit);
		free(bitmapb->map[i_directorio->i_directos[i]]);
		i_directorio->i_directos[i] = -1;
	}
	free(vacio);
	return last;
}

// Buscamos la entrada de directorio por su nombre y el inodo del directorio donde suponemos está
void remove_dentry (char *nombre, struct inode_fs *i_directorio, struct block_bitmap_fs * bitmapb)
{
	int i = 0;
	int encontrada = 0;
	struct directory_entry *d_entry = malloc(sizeof(struct directory_entry));
	struct directory_entry *last_entry = malloc(sizeof(struct directory_entry));

	// Recorremos i_directos mientras sea un bloque válido y aún no hayamos encontrado la entrada
	while ((i < N_DIRECTOS) && (i_directorio->i_directos[i] != -1) && !encontrada) {
		memcpy(d_entry, bitmapb->map[i_directorio->i_directos[i]]+ (sizeof(struct directory_entry)*2), sizeof(struct directory_entry));
		// Recorremos cada entrada de directorio (son 32)
		for (int j = 2; j < 32 && d_entry->inode != NULL && !encontrada; j++) {
			// Nos traemos a d_entry el contenido de este bloque (que será una directory entry)
			memcpy(d_entry, bitmapb->map[i_directorio->i_directos[i]] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
			if (strcmp(d_entry->name, nombre) == 0) {
				// La hemos encontrado, ahora seteamos a 0 los bloques que ocupaba. j-1 porque en while ya tenemos la 1ª entrada
				last_entry = search_last_entry(i_directorio, bitmapb);
				if(strcmp(d_entry->name,last_entry->name)){
					memcpy(bitmapb->map[i_directorio->i_directos[i]] + (sizeof(struct directory_entry)*j), last_entry, sizeof(struct directory_entry));
				}
				encontrada = 1;   // Para salir en cuanto la encontremos

			}
		}

		i++;
	}

	// Comprobamos si hemos encontrado la entrada
	if (!encontrada) {
		printf("Entrada no encontrada en directorio :[ \n");
	} else {
		printf("Entrada encontrada y borrada :D \n");
	}
	free(d_entry);   // Ya no apunta a nada
	free(last_entry);
}

void remove_inode (struct inode_fs *inode, struct inode_bitmap_fs *bitmap)
{
	struct directory_entry *vacio = malloc(sizeof(struct directory_entry));
	// Lmpiamos inodo

	// Seteamos a 0 en bitmap de inodos
	int byte = inode->i_num / 8;  // Obtenemos byte del bitmap
	int bit = 7 - (inode->i_num % 8);
	bitmap->bitmap[byte] = bitmap->bitmap[byte] &~ (1 << bit);
	inode=vacio;
	free(vacio);
}

// Buscamos en el directorio el fichero por su nombre y lo eliminamos
void rm (char *nombre, struct inode_fs *current_directory, struct inode_bitmap_fs *bitmap, struct block_bitmap_fs *bitmapb)
{
    // Buscamos el inodo desde el directorio actual
    struct inode_fs *i_buscado = inode_search(nombre, current_directory, bitmapb);

    if (i_buscado == NULL) {
        printf("No existe el archivo %s\n", nombre);
        return;
    }
    if(i_buscado->i_type != 'f'){
    	printf("No es un fichero\n");
    }else{
    	printf("Existe el archivo %s, con nº inodo: %d\n", nombre, i_buscado -> i_num);

    	        // Ponemos bits de bitmap y bitmapb a 0 (podemos crear una funcion remove_entry para ello)
    	        //Para fichero:
    	        	//Guardamos en un aux el inodo que vamos a eliminar
    	        	//Borramos la entrada del inodo en el directorio --> LO HE HECHO USANDO EL NOMBRE
    	        	//si tam > 0 vamos bloque por bloque seteando a 0 el bitmap y free(bloque)
    	        	//setear a 0 en bitmap[num_inodo] --> HECHO
    	        	//free(aux);

    	    // Eliminamos la entrada del directorio actual a partir de su nombre
    	    remove_dentry(nombre, current_directory, bitmapb);

    	    // Eliminaos el inodo correspondiente
    	    clean_inode(i_buscado,bitmapb);
    	    remove_inode(i_buscado, bitmap);
    }

}
*/


