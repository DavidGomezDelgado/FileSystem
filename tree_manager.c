#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

private struct inode_fs *existe_inode(char *name, struct inode_fs entry){
	if(entry->inode->i_type == 'd'){
		return inode_search(name,(* entry)->inode);
	}else if(strcmp(name, entry->name) == 0){
		return entry->inode;
	}else if(entry == NULL){
		return NULL;
	}
}

struct inode_fs *inode_search(char *name, struct inode_fs *directory){
	inode_fs inodo = malloc(sizeof(struct inode_fs));
	inodo = NULL;
	int i,j;
	directory_entry *entry = malloc(sizeof(struct directory_entry);
	//Comenzamos desde el directorio actual (directory) 
	
	//Miramos sus punteros directos y buscamos de manera recursiva
	
	//devolvemos el inodo encontrado
	for(i = 0; i < N_DIRECTOS && directory->i_directos[i] != NULL; i++){
		memcpy(entry, directory.i_directos[i], sizeof(struct directory_entry));
		for(j = 0; i < 32 && (*entry) == NULL; i++){
			inodo = existe_inode(name, (*entry));
			memcpy(entry, directory.i_directos[i] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
		}
	}
	//Buscamos en el puntero indirecto
	if(inodo == NULL && i < (N_DIRECTOS-1)) {
		memcpy(entry, directory.i_simple_ind[1], sizeof(struct directory_entry));
		inodo = existe_inode(name, (*entry));
	}
	
	
	return inodo;
}


/*char *path[24] = {"/"};
	char aux[24];
	int i;
	int n_aux = 0;
	int n_inodes = 1;
	//Guardar en un array el path
	for(i = 1; i < name.size(); i++){
		if (name[i] == NULL){
			i = n.size();
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
