#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

int main(){
	int i;

	// Creamos mapas de bits
	struct inode_bitmap_fs *bitmap = malloc(sizeof(struct inode_bitmap_fs));
	struct block_bitmap_fs *bitmapb = malloc(sizeof(struct block_bitmap_fs));

	fflush(stdout);
	printf("Creando algunos inodos...\n");
	fflush(stdout);

	// Creamos inodo /
	struct inode_fs *raiz = create_root(bitmap, bitmapb);
	printf("Tam: %d\n Type: %c\n Numero: %d\n",(*raiz).i_tam, (*raiz).i_type, (*raiz).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", raiz->i_directos[i]);
	fflush(stdout);
	printf("BloqueReal: %llud \n", bitmapb->map[raiz->i_directos[0]]);
	fflush(stdout);
	read_directory("/", raiz, bitmapb);

	// Creamos algunos inodos fichero
	struct inode_fs *inodo = create_inode('f', "test1", bitmap, bitmapb);
	printf("Tam: %d\n Type: %c\n Numero: %d\n",(*inodo).i_tam, (*inodo).i_type, (*inodo).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", inodo->i_directos[i]);
	fflush(stdout);

	struct inode_fs *inodo2 = create_inode('f', "test2", bitmap,bitmapb);
	printf("Tam: %d\n Type: %c\n Numero: %d\n",(*inodo2).i_tam,(*inodo2).i_type, (*inodo2).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", inodo2->i_directos[i]);
	fflush(stdout);

	// Creamos un inodo directorio
	struct inode_fs *inodo3 = create_inode('d', "testd", bitmap,bitmapb);
	printf("Tam: %d\n Type: %c\n Numero: %d\n",(*inodo3).i_tam, (*inodo3).i_type, (*inodo3).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", inodo3->i_directos[i]);
	fflush(stdout);
	printf("BloqueReal: %llud \n", bitmapb->map[inodo3->i_directos[0]]);
	fflush(stdout);

	// Creamos un fichero en / a partir de inodo raíz
	touch("testtouch", 'f', "/", raiz, bitmap, bitmapb);
	struct inode_fs *aux = inode_search("testtouch", raiz, bitmapb);
	printf("Inodo buscado: \n Tam: %d\n Type: %c\n Numero: %d\n",(*aux).i_tam, (*aux).i_type, (*aux).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", aux->i_directos[i]);
	fflush(stdout);

	// Creamos un directorio en / a partir de inodo raíz
	touch("testtouchd", 'd', "/", raiz, bitmap, bitmapb);
	struct inode_fs *aux2 = inode_search("testtouchd", raiz, bitmapb);
	printf("Inodo buscado d: \n Tam: %d\n Type: %c\n Numero: %d\n",(*aux2).i_tam, (*aux2).i_type, (*aux2).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", aux2->i_directos[i]);
	fflush(stdout);

	// Mostramos contenido del directorio /
	struct directory_entry *raizdir = malloc(sizeof(struct directory_entry)) ;
	for(int i = 0; i < 4; i++){  // Ejemplo con 4 bloques
		memcpy(raizdir, (bitmapb->map[raiz->i_directos[0]]+sizeof(struct directory_entry)*i), sizeof(struct directory_entry));
		printf("Entrada raiz %d: Nombre: %s, %d\n",i, raizdir->name, raizdir->inode->i_num);
	}
	fflush(stdout);
	free(raizdir);

	// Mostramos contenido del directorio testtouchd
	struct directory_entry *directorio = malloc(sizeof(struct directory_entry)) ;
	for(int i = 0; i < 2; i++){
		memcpy(directorio , (bitmapb->map[aux2->i_directos[0]]+sizeof(struct directory_entry)*i), sizeof(struct directory_entry));
		printf("Entrada testtouchd %d: Nombre: %s, %d\n",i, directorio->name, directorio->inode->i_num);
	}
	fflush(stdout);
	free(directorio);

	// Creamos subdirectorio a partir del directorio testtouchd y mostramos i_directos
	touch("testtouchd2", 'd', "testtouchd", aux2, bitmap, bitmapb);
	struct inode_fs *aux3 = inode_search("testtouchd2", aux2, bitmapb);
	printf("Inodo buscado d2: \n Tam: %d\n Type: %c\n Numero: %d\n",(*aux3).i_tam, (*aux3).i_type, (*aux3).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", aux3->i_directos[i]);
	fflush(stdout);

	// Mostramos contenido del directorio testtouchd2
	struct directory_entry *directorio2 = malloc(sizeof(struct directory_entry)) ;
	for(int i = 0; i < 2; i++){
		memcpy(directorio2 , (bitmapb->map[aux3->i_directos[0]]+sizeof(struct directory_entry)*i), sizeof(struct directory_entry));
		printf("Entrada testtouchd2 %d: Nombre: %s, %d\n",i, directorio2->name, directorio2->inode->i_num);
	}
	fflush(stdout);
	free(directorio2);

	// Escribimos en testtouch  TODO
	file_edit("Vamos a probarlo", "testtouch", raiz, bitmapb);
	printf("Inodo buscado: \n Tam: %d\n Type: %c\n Numero: %d\n",(*aux).i_tam, (*aux).i_type, (*aux).i_num);
	fflush(stdout);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", aux->i_directos[i]);
	fflush(stdout);
	printf("BloqueReal del fichero: %llud \n", bitmapb->map[aux->i_directos[0]]);
	fflush(stdout);

	int j;
	read_file("testtouch", raiz, bitmapb);
	file_edit("", "testtouch", raiz, bitmapb);
	read_file("testtouch", raiz, bitmapb);

	// prueba rm
	rm("hola", raiz, bitmap, bitmapb);
	rm("testtouch", raiz, bitmap, bitmapb);
	rm("testtouch", raiz, bitmap, bitmapb);

	touch("hola","f", "/", raiz, bitmap, bitmapb);
	read_directory("/", raiz, bitmapb);
	fflush(stdout);

}
