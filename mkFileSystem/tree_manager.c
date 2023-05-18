#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

struct inode_fs *existe_inode(char *name, struct directory_entry *entry, struct block_bitmap_fs *bitmapb){
	if(strcmp(name, entry->name) == 0){
		return entry->inode;
	}else {
		return NULL;
	}
}

struct inode_fs *inode_search(char *name, struct inode_fs *directory, struct block_bitmap_fs *bitmapb){
	struct inode_fs *inodo;
	inodo = NULL;
	int i,j, k, finDirectos = 1, esIgual = 0;
	struct directory_entry *entry = malloc(sizeof(struct directory_entry));
	struct directory_entry *aux = malloc(sizeof(struct directory_entry));
	struct directory_entry *aux2 = malloc(sizeof(struct directory_entry));
	//Comenzamos desde el directorio actual (directory) 
	
	//Miramos sus punteros directos y buscamos de manera recursiva
	
	//Estamos preguntando por si mismo?
	//memcpy(aux, bitmapb->map[directory->i_directos[0]]+sizeof(struct directory_entry), sizeof(struct directory_entry));
	//if(aux->inode->i_directos[0] != -1){
		//memcpy(aux2, bitmapb->map[aux->inode->i_directos[0]] + (sizeof(struct directory_entry)*2), sizeof(struct directory_entry));
			for(j = 2; j < 32 && aux2->inode != NULL; j++){
				if(aux2->inode->i_type == 'd'){
					if((*aux2).inode != NULL)
						inodo = existe_inode(name, aux2,bitmapb);
					if(inodo != NULL){
						esIgual = 1;
						break;
					}
				}
				//if(j+1 < 32)
				//memcpy(aux2, bitmapb->map[aux->inode->i_directos[0]] + (sizeof(struct directory_entry)*(j+1)), sizeof(struct directory_entry));
			//}
		}
		for(i = 1; i < N_DIRECTOS && finDirectos; i++){
			if(directory->i_directos[i] != -1){
				for(j = 0; j < 31; j++){
					//memcpy(aux2, bitmapb->map[aux->inode->i_directos[i]] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
					if((*aux2).inode != NULL)
						inodo = existe_inode(name, aux2,bitmapb);
					if(inodo != NULL){
							esIgual = 1;
							break;
					}

				}
			}
			if(inodo != NULL) break;
		}

	if(!esIgual){
		//Intentar implementar en otra función auxiliar
			//devolvemos el inodo encontrado
			//Eficiencia para saltarnos . y ..
			if(directory->i_directos[0] != -1){
				for(j = 2; j < 31; j++){
						//memcpy(entry, bitmapb->map[directory->i_directos[0]] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
					if((*entry).inode != NULL)
						inodo = existe_inode(name, entry,bitmapb);
					if(inodo != NULL){
						break;
					}

				}
			}
			for(i = 1; i < N_DIRECTOS && finDirectos; i++){
				if(directory->i_directos[i] != -1){
					for(j = 0; j < 31; j++){
						//memcpy(entry, bitmapb->map[directory->i_directos[i]] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
						if(entry->inode != NULL)
						inodo = existe_inode(name, entry,bitmapb);
						if(inodo != NULL){
								break;
						}

					}
				}
				if(directory->i_directos[i+1] == -1) finDirectos = 0;
				if(inodo != NULL) break;
			}
	}


	//TODO
	//Buscamos en el puntero indirecto
	/*if(inodo == NULL && i < (N_DIRECTOS-1)) {
		//llegamos a un bloque con punteros a otros bloques
		for(k = 0; k < 31; k++){
			inodo = existe_inode(name, (*entry), bitmapb);
		}
	}*/
	
	free(entry);
	free(aux);
	free(aux2);
	//return inodo;
	return NULL;
}

/*struct inode_fs *_inode_search(char *name, char *path, struct inode_fs current_directory, struct block_bitmap_fs bitmapb){

	return NULL;
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
