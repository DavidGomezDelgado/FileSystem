#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

void file_edit(char *contenido, char *nombre, struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
	if(strcmp(contenido, "") != 0){
		char* cadena = malloc(BLOCK_SIZE);
			struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
			inodo = inode_search(nombre, directory, bitmapb);
			int i, j, buffer = strlen(contenido)+1, bloques;
			//char *posBlock;
			if(inodo == NULL){
				printf("No existe el fichero\n");
				return;
			}
			//borramos contenido
			clean_inode(inodo, bitmapb);

			//escribir el contenido
			inodo->i_tam = buffer;
			if(buffer < BLOCK_SIZE){
				for(j = 0; j< buffer; j++){
					cadena[j] = contenido[j];
				}
			}
			cadena[buffer] = '\o';

			if(buffer%BLOCK_SIZE == 0){
					bloques = (buffer)/BLOCK_SIZE;
			}else{
					bloques = (buffer)/BLOCK_SIZE+1;
			}

			//Reservamos el numero de bloques
			for(i = 0; i < bloques; i++){
				inodo->i_directos[i] = free_block(bitmapb);
			}

			i = 0;
			for(j = 0; j < bloques; j++){
				memcpy(bitmapb->map[inodo->i_directos[i]], cadena, BLOCK_SIZE);
				i++;
			}
			free(cadena);
	}else{
		printf("Error, contenido es unca cadena vacia");
	}

	fflush(stdout);
	return;
}

char *read_file(char *nombre,struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
	struct inode_fs *inodo;
	int i;
	inodo = inode_search(nombre, directory,bitmapb);
	if(inodo == NULL){
		printf("El fichero no existe en el directorio");
	}else{
		char *contenidoTotal;
		contenidoTotal[0] = '\0';
		for(i = 0; i < N_DIRECTOS && inodo->i_directos[i] != -1; i++){
			strcat(contenidoTotal,bitmapb->map[inodo->i_directos[i]]);  // Creo que no estamos accedeindo a caracteres
		}
		printf("La cadena es: \n%s\n", contenidoTotal);
		return contenidoTotal;
	}
	fflush(stdout);
	return NULL;//DEVOLVER ERROR
}


/* Renombramos un fichero (o directorio) a partir de su nombre (name) en su directorio (directory) */
/* Sólo cambiamos el nombre del registro de directorio (NO BORRAMOS ENTRY Y CREAMOS NUEVA, QUE SERÍA LO SUYO) */

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
		memcpy(entry, bitmapb -> map[directory -> i_directos[i] + offset], sizeof(struct directory_entry));

		for (int j = 0; j < 32 && (entry -> inode != NULL) && !encontrado; j++) {
			if (strcmp(entry -> name, name) == 0) {
				// Hemos encontrado la entrada, cambiamos su nombre
				strcpy(entry -> name, new_name);
				encontrado = 1;
			}

			// MEJORA: en vez de cambiar el nombre a la entrada, eliminarla (setear a 0) y añadir nueva con new_name al mismo inodo
		}
	}

	free(entry);
}







