#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/* Buscamos en el directorio el fichero por su nombre y lo eliminamos */
void rm (char *nombre, struct inode_fs *current_directory, struct inode_bitmap_fs *bitmap, struct block_bitmap_fs *bitmapb)
{

    // Buscamos el inodo desde / según el nombre
    struct inode_fs *i_buscado = inode_search(nombre, current_directory, bitmapb);

    if (i_buscado == NULL) {
        printf("No existe el archivo %s\n", nombre);
    } else {
        printf("Existe el archivo %s, con nº inodo: %d\n", nombre, i_buscado -> i_num);

        // Ponemos bits de bitmap y bitmapb a 0 (podemos crear una funcion remove_entry para ello)
        //Para fichero:
        	//Guardamos en un aux el inodo que vamos a eliminar
        	//Borramos la entrada del inodo en el directorio
        	//si tam > 0 vamos bloque por bloque seteando a 0 el bitmap y free(bloque)
        	//setear a 0 en bitmap[num_inodo]
        	//free(aux);

    }
}
