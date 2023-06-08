#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "filesystem.h"

// MODIFICAR SEGÚN FUNCIÓN WRITE DE FUSE
size_t file_edit(const char *contenido, char *path, size_t size, off_t offset, filesystem_t *private_data){
	char* cadena;
	struct inode_fs *inode;
	unsigned long i, j, k, buffer = size, bloques;
	char path_aux[70], dir[70], base[70];

	strcpy(path_aux, path);

	// Obtenemos nombre del fichero o directorio y path hasta el directorio padre
	strcpy(base, basename(path_aux));  // Nos sirve para buscarlo en el inodo padre
	strcpy(dir, dirname(path_aux));

	// Obtenemos inodo del fichero o directorio
	inode = inode_search_path(path, private_data);

	if(strcmp(contenido, "") != 0){
		
		clean_inode(inode, private_data);
		if(buffer > BLOCK_SIZE*10-1){
			buffer = BLOCK_SIZE*10-1;
		}
		inode->i_tam = buffer;
		if(buffer < BLOCK_SIZE){
			
			inode->i_directos[0] = free_bit(&(private_data->block_bitmap));
			cadena = (char *) private_data->block[inode->i_directos[0] - private_data->superblock->reserved_block];
			//for(j = offset; j< buffer-1; j++){
			//	cadena[j] = contenido[j];
			//}
			memcpy(cadena, contenido, buffer);
		}else{
			if(buffer%BLOCK_SIZE == 0){
				bloques = buffer/BLOCK_SIZE;
			}else{
				bloques = buffer/BLOCK_SIZE  +1;
			}
			for(i = 0; i < bloques && i < N_DIRECTOS; i++){
				 inode->i_directos[i] = free_bit(&(private_data->block_bitmap));
			}
			for(i = 0; i < bloques - 1 && i < N_DIRECTOS; i++){
				cadena = (char *) private_data->block[inode->i_directos[i] - private_data->superblock->reserved_block];
				for(j = 0; j < BLOCK_SIZE; j++){
					cadena[j] = contenido[BLOCK_SIZE*i + j];
				}
				memcpy(private_data->block[inode->i_directos[i] - private_data->superblock->reserved_block], cadena, BLOCK_SIZE);
			}
			//En la última iteración la cadena puede ser menor que BLOCK_SIZE
			cadena = (char *) private_data->block[inode->i_directos[bloques-1] - private_data->superblock->reserved_block];
			for(j = 0, k = 0; j < buffer && k < BLOCK_SIZE ; j++, k++){
				cadena[j] = contenido[BLOCK_SIZE*(bloques-1) + k];
			}
		}
		cadena[buffer-1] = '\n';

	}else{
		printf("No hay contenido para escribir\n");
	}
	fflush(stdout);
	return buffer;
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

// MODIFICAR SEGÚN FUNCIÓN READ DE FUSE
char *read_file(char *name, struct inode_fs *directory, filesystem_t *private_data){
	unsigned long i = 0, finCadena = 1;
	char *contenido;
	struct inode_fs *inodo = inode_search(name, directory, private_data);
	char *cadenaFinal;
	if(inodo->i_type == 'd'){
		return "Esto es un directorio, no es un fichero con contenido";
	}
	if(inodo == NULL) printf("El fichero no existe en el directorio");
	else{
		cadenaFinal =  (char *)malloc(BLOCK_SIZE);
		for(i = 0; i < N_DIRECTOS && inodo->i_directos[i] != 0 && finCadena; i++){
			//if(private_data->block[inodo->i_directos[i] - private_data->superblock->reserved_block] == NULL) finCadena = 0;
			//else {
				contenido = (char *) private_data->block[inodo->i_directos[i] - private_data->superblock->reserved_block];
				strncat(cadenaFinal, contenido, BLOCK_SIZE);
			//}

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









