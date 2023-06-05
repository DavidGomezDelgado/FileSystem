#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "filesystem.h"

void file_edit(char *contenido, char *name, struct inode_fs *directory, filesystem_t *private_data){
	char* cadena;
	unsigned long i, j, k, buffer = strlen(contenido) + 1, bloques;
	if(buffer > BLOCK_SIZE*10){
		printf("La cadena que quiere introducir es demasiado grande\n");
		return;
	}
	struct inode_fs *inodo =  inode_search(name, directory, private_data);;
	if(inodo->i_type == 'd'){
		printf("Esto es un directorio, no es un fichero con contenido");
		return;
	}
	if(strcmp(contenido, "") != 0){
		if(inodo == NULL){
			printf("No existe el fichero \n");
			return;
		}
		clean_inode(inodo, private_data);
		inodo->i_tam = buffer;
		if(buffer < BLOCK_SIZE){
			inodo->i_directos[0] = free_bit(&(private_data->block_bitmap));
			cadena = (char *) private_data->block[inodo->i_directos[0] - private_data->superblock->reserved_block];
			for(j = 0; j< buffer-1; j++){
				cadena[j] = contenido[j];
			}
		}else{
			if(buffer%BLOCK_SIZE == 0){
				bloques = buffer/BLOCK_SIZE;
			}else{
				bloques = buffer/BLOCK_SIZE  +1;
			}
			for(i = 0; i < bloques && i < N_DIRECTOS; i++){
				 inodo->i_directos[i] = free_bit(&(private_data->block_bitmap));
			}
			for(i = 0; i < bloques - 1 && i < N_DIRECTOS; i++){
				cadena = (char *) private_data->block[inodo->i_directos[i] - private_data->superblock->reserved_block];
				for(j = 0; j < BLOCK_SIZE; j++){
					cadena[j] = contenido[BLOCK_SIZE*i + j];
				}
				memcpy(private_data->block[inodo->i_directos[i] - private_data->superblock->reserved_block], cadena, BLOCK_SIZE);
			}
			//En la última iteración la cadena puede ser menor que BLOCK_SIZE
			cadena = (char *) private_data->block[inodo->i_directos[bloques-1] - private_data->superblock->reserved_block];
			for(j = 0, k = 0; j < buffer && k < BLOCK_SIZE ; j++, k++){
				cadena[j] = contenido[BLOCK_SIZE*(bloques-1) + k];
			}
		}
		cadena[buffer-1] = '\0';

	}else{
		printf("No hay contenido para escribir\n");
	}
	fflush(stdout);
	return;
}

//BORRAR
//void file_edit(char *contenido, char *nombre, struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
//	if(strcmp(contenido, "") != 0){
//		char* cadena = malloc(BLOCK_SIZE);
//			struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
//			inodo = inode_search(nombre, directory, bitmapb);
//			int i, j, buffer = strlen(contenido)+1, bloques;
//			//char *posBlock;
//			if(inodo == NULL){
//				printf("No existe el fichero\n");
//				return;
//			}
//			//borramos contenido
//			clean_inode(inodo, bitmapb);
//
//			//escribir el contenido
//			inodo->i_tam = buffer;
//			if(buffer < BLOCK_SIZE){
//				for(j = 0; j< buffer; j++){
//					cadena[j] = contenido[j];
//				}
//			}
//			cadena[buffer] = '\o';
//
//			if(buffer%BLOCK_SIZE == 0){
//					bloques = (buffer)/BLOCK_SIZE;
//			}else{
//					bloques = (buffer)/BLOCK_SIZE+1;
//			}
//
//			//Reservamos el numero de bloques
//			for(i = 0; i < bloques; i++){
//				inodo->i_directos[i] = free_block(bitmapb);
//			}
//
//			i = 0;
//			for(j = 0; j < bloques; j++){
//				memcpy(bitmapb->map[inodo->i_directos[i]], cadena, BLOCK_SIZE);
//				i++;
//			}
//			free(cadena);
//	}else{
//		printf("Error, contenido es unca cadena vacia");
//	}
//
//	fflush(stdout);
//	return;
//}

char *read_file(char *name, char *cadenaFinal,struct inode_fs *directory, filesystem_t *private_data){
	unsigned long i = 0, finCadena = 1;
	char *contenido;
	struct inode_fs *inodo = inode_search(name, directory, private_data);
	//~ char *cadenaFinal = NULL;
	if(inodo->i_type == 'd'){
		printf ("Esto es un directorio, no es un fichero con contenido");
	}
	if(inodo == NULL)  printf ("El fichero no existe en el directorio");
	else{
		//~ cadenaFinal =  (char *)malloc(BLOCK_SIZE);
		for(i = 0; i < N_DIRECTOS && inodo->i_directos[i] != 0 && finCadena; i++){
			if(private_data->block[inodo->i_directos[i] - private_data->superblock->reserved_block] == NULL) finCadena = 0;
			else {
				contenido = (char *) private_data->block[inodo->i_directos[i] - private_data->superblock->reserved_block];
				strncat(cadenaFinal, contenido, BLOCK_SIZE);
			}

		}
		//printf("La cadena es: \n%s\n", cadenaFinal);
	}
	return cadenaFinal;
}

//BORRAR
//char *read_file(char *nombre,struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
//	struct inode_fs *inodo;
//	int i;
//	inodo = inode_search(nombre, directory,bitmapb);
//	if(inodo == NULL){
//		printf("El fichero no existe en el directorio");
//	}else{
//		char *contenidoTotal;
//		contenidoTotal[0] = '\0';
//		for(i = 0; i < N_DIRECTOS && inodo->i_directos[i] != -1; i++){
//			strcat(contenidoTotal,bitmapb->map[inodo->i_directos[i]]);  // Creo que no estamos accedeindo a caracteres
//		}
//		printf("La cadena es: \n%s\n", contenidoTotal);
//		return contenidoTotal;
//	}
//	fflush(stdout);
//	return NULL;//DEVOLVER ERROR ¿o cadena vacía?
//}









