#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/* Buscamos en el directorio el fichero por su nombre y lo eliminamos */
void rm (char *nombre, char *directory, struct inode_fs *raiz, struct inode_bitmap_fs *bitmap, struct block_bitmap_fs *bitmapb)
{
    // Buscamos directorio según directory
    struct inode_fs *d_buscado = inode_search(nombre, raiz, bitmapb);

    if (d_buscado == NULL) {
        printf("No existe el directorio\n");
    } else {
        printf("Existe el directorio %s, con nº inodo: %d\n", directory, d_buscado -> i_num);
    }

    // Buscamos el inodo desde / según el nombre
    struct inode_fs *i_buscado = inode_search(nombre, raiz, bitmapb);

    if (i_buscado == NULL) {
        printf("No existe el archivo %s\n", nombre);
    } else {
        printf("Existe el archivo %s, con nº inodo: %d\n", nombre, i_buscado -> i_num);

        // Ponemos bits de bitmap y bitmapb a 0 (podemos crear una funcion remove_entry para ello)

    }
}