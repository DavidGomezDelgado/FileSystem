#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

struct inode_fs *existe_inode(char *name, struct directory_entry entry){
	if(entry.inode->i_type == 'd'){
		return inode_search(name,entry.inode);
	}else if(strcmp(name, entry.name) == 0){
		return entry.inode;
	}else {
		return NULL;
	}
}

struct inode_fs *inode_search(char *name, struct inode_fs *directory){
	struct inode_fs *inodo = malloc(sizeof(struct inode_fs));
	inodo = NULL;
	int i,j, k;
	struct directory_entry *entry = malloc(sizeof(struct directory_entry));
	struct directory_entry *aux = malloc(sizeof(struct directory_entry));
	//Comenzamos desde el directorio actual (directory) 
	
	//Miramos sus punteros directos y buscamos de manera recursiva
	
	//devolvemos el inodo encontrado
	for(i = 0; i < N_DIRECTOS; i++){
		if(directory->i_directos[i] != -1){
			for(j = 0; j < 32; j++){
				memcpy(entry, directory->i_directos[i] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
				if(entry->inode != NULL){
					inodo = existe_inode(name, (*entry));
					if(inodo != NULL){
						break;
					}
				}
			}
		}
		if(inodo != NULL)break;
		if(directory->i_directos[i+1] == NULL) i = N_DIRECTOS;
	}

	//TODO
	//Buscamos en el puntero indirecto
	if(inodo == NULL && i < (N_DIRECTOS-1)) {
		//llegamos a un bloque con punteros a otros bloques
		for(k = 0; k < 31; k++){
			inodo = existe_inode(name, (*entry));
		}
	}
	
	free(entry);
	free(aux);
	return inodo;
}


/*char *path[24] = {"/"};
	char aux[24];
	int i;
	int n_aux = 0;
	int n_inodes = 1;
	//Guardar en un array el path
	for(i = 1; i < name->size(); i++){
		if (name[i] == NULL){
			i = n->size();
		}else if(!strcmp(name[i],'/')){
			n_inodes++;
			strcpy(path[n_inodes], aux);
			strcpy(aux, "");
			n_aux = 0;
		}else{
			aux[n_aux] = name[i];
			n_aux++;
		}	
	}*/
