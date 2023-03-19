#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"




//Preguntar
//Es muy reutilizable
Inode buscarInode(char *nombre,struct Inode raiz, struct Block block){
	Inode objetivo;
	int i = 0;
	int encontrado = 0;
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
