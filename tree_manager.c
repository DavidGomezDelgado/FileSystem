#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"


struct inode_fs *inode_search(char *name, struct inode_fs *directory){
	struct inode_fs inodo;
	char *path[24] = {"/"};
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
	}
	
	//Comenzamos desde raiz
	
	
	//Miramos sus punteros directos y buscamos de manera recursiva
	
	//devolvemos el inodo encontrado
}
