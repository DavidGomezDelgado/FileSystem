#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

/* Buscamos la entrada de directorio por su nombre y el inodo del directorio donde suponemos está */
void remove_dentry (char *nombre, struct inode_fs *i_directorio, struct block_bitmap_fs * bitmapb)
{
	int i = 0;
	int encontrada = 0;
	struct directory_entry *d_entry = malloc(sizeof(struct directory_entry));

	// Recorremos i_directos mientras sea un bloque válido y aún no hayamos encontrado la entrada
	while ((i < N_DIRECTOS) && (d_entry->inode->i_directos[i] != -1) && !encontrada) {

		// Nos traemos a d_entry el contenido de este bloque (que será una directory entry)
		memcpy(d_entry, bitmapb->map[i_directorio->i_directos[i]] + (sizeof(struct directory_entry)*i), sizeof(struct directory_entry));

		// Recorremos cada entrada de directorio (son 32)
		for (int j = 1; j < 32 && d_entry->inode != NULL && !encontrada; j++) {
			if (strcmp(d_entry->name, nombre) == 0) {
				// La hemos encontrado, ahora seteamos a 0 los bloques que ocupaba. j-1 porque en while ya tenemos la 1ª entrada
				memset(bitmapb->map[i_directorio->i_directos[i]] + (sizeof(struct directory_entry)*(j-1)), 0);   // ¿Seteamos a -1 o 0?
				encontrada = 1;   // Para salir en cuanto la encontremos
			} else {
				// No la hemos encontrado, nos traemos la siguiente entrada
				memcpy(d_entry, bitmapb->map[i_directorio->i_directos[i]] + (sizeof(struct directory_entry)*j), sizeof(struct directory_entry));
			}
		}

		i++;
	}

	// Comprobamos si hemos encontrado la entrada
	if (!encontrada) {
		printf("Entrada no encontrada en directorio :[ \n");
	} else {
		printf("Entrada encontrada y borrada :D \n");
		d_entry = NULL;   // Ya no apunta a nada
	}

	free(d_entry);
}

void remove_inode (struct inode_fs *inode, struct inode_bitmap_fs *bitmap)
{
	// Lmpiamos inodo

	// Seteamos a 0 en bitmap de inodos
	int byte = inode->i_num / 8;  // Obtenemos byte del bitmap
	int bit = 7 - (inode->i_num % 8);
	bitmap->bitmap[byte] &= (0 << bit);   // DUDA CON OPERACIÓN

	free(inode);
}

/* Buscamos en el directorio el fichero por su nombre y lo eliminamos */
void rm (char *nombre, struct inode_fs *current_directory, struct inode_bitmap_fs *bitmap, struct block_bitmap_fs *bitmapb)
{
    // Buscamos el inodo desde el directorio actual
    struct inode_fs *i_buscado = inode_search(nombre, current_directory, bitmapb);

    if (i_buscado == NULL) {
        printf("No existe el archivo %s\n", nombre);
        return;
    }

    printf("Existe el archivo %s, con nº inodo: %d\n", nombre, i_buscado -> i_num);

        // Ponemos bits de bitmap y bitmapb a 0 (podemos crear una funcion remove_entry para ello)
        //Para fichero:
        	//Guardamos en un aux el inodo que vamos a eliminar
        	//Borramos la entrada del inodo en el directorio --> LO HE HECHO USANDO EL NOMBRE
        	//si tam > 0 vamos bloque por bloque seteando a 0 el bitmap y free(bloque)
        	//setear a 0 en bitmap[num_inodo] --> HECHO
        	//free(aux);

    // Eliminamos la entrada del directorio actual a partir de su nombre
    remove_dentry(nombre, current_directory, bitmapb);

    // Eliminaos el inodo correspondiente
    remove_inode(i_buscado, bitmap);
}
