#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filesystem.h"



static struct Inode *_buscarInode(char *nombre, struct Inode dir, uint8_t *encontrado){
	
	return NULL;
}

//Es muy reutilizable
static struct Inode *buscarInode(char *nombre,struct Inode raiz){
	//Inode objetivo;
	int i = 0;
	uint8_t encontrado = 0;
	while(i < N_DIRECTOS && encontrado){
		
		
		
		i++;
	}

	return objetivo;
}

void crearFichero(Inode raiz, char nombreFich){
	int libre;
	int i = 0;
	while(libre){
		//Entrar en primer bloque
		//¿Tiene posición libre?
			//Sí -> rellenar
			//No->siguiente bloque
		//Encontrar un bloque vacío
			//Inicializar en bitmap y rellenar los campos del bloque.
	}
	
}
