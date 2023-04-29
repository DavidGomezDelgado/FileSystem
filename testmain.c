#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

int main(){
	int i;
	struct inode_bitmap_fs *bitmap = malloc(sizeof(struct inode_bitmap_fs));
	struct inode_bitmap_fs *bitmapb = malloc(sizeof(struct block_bitmap_fs));
	struct inode_fs *raiz = create_root(bitmap, bitmapb);
	printf("Tam: %d\n Nombre: %s\n Type: %c\n Numero: %d\n",(*raiz).i_tam, (*raiz).i_name, (*raiz).i_type, (*raiz).i_num);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", raiz->i_directos[i]);
	struct inode_fs *inodo = create_inode('f', "test1", bitmap, bitmapb);
	printf("Tam: %d\n Nombre: %s\n Type: %c\n Numero: %d\n",(*inodo).i_tam, (*inodo).i_name, (*inodo).i_type, (*inodo).i_num);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", inodo->i_directos[i]);
	struct inode_fs *inodo2 = create_inode('f', "test2", bitmap,bitmapb);
	printf("Tam: %d\n Nombre: %s\n Type: %c\n Numero: %d\n",(*inodo2).i_tam, (*inodo2).i_name, (*inodo2).i_type, (*inodo2).i_num);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", inodo2->i_directos[i]);


	struct inode_fs *inodo3 = create_inode('d', "testd", bitmap,bitmapb);
	printf("Tam: %d\n Nombre: %s\n Type: %c\n Numero: %d\n",(*inodo3).i_tam, (*inodo3).i_name, (*inodo3).i_type, (*inodo3).i_num);
	for(i = 0; i < N_DIRECTOS; i++) printf("%d\n", inodo3->i_directos[i]);

	touch("testtouch", 'f', '/', raiz, bitmap, bitmapb);
	struct inode_fs *aux = inode_search("testtouch", raiz);
	printf("Inodo buscado: \n Tam: %d\n Nombre: %s\n Type: %c\n Numero: %d\n",(*aux).i_tam, (*aux).i_name, (*aux).i_type, (*aux).i_num);

}
