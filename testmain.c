#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

int main(){
	struct inode_bitmap_fs *bitmap = malloc(sizeof(struct inode_bitmap_fs));
	struct inode_fs inodo = create_inode('f', "test", bitmap);
	 printf("Tam: %d\n Nombre: %s\n Type: %c\n Numero: %d\n",inodo->i_tam, inodo->i_nombre, inodo->i_type, inodo->i_num);
}
