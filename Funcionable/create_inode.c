#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"


///* Creación de un inodo */
void create_inode(char type, filesystem_t *private_data){
	int numi = free_bit(&private_data->inode_bitmap);
	if(numi == private_data->inode_bitmap.size * 8 +1){
		printf("NO HAY MÁS ESPACIO DE INODOS");
		exit(EXIT_FAILURE);
	}
	private_data->inode[numi].i_num = numi;
	private_data->inode[numi].i_type = type;
	private_data->inode[numi].i_permisos = 666;
	if(type == 'd'){
		private_data->inode[numi].i_links = 2;
		private_data->inode[numi].i_tam = sizeof(struct directory_entry) * 2;
		private_data->inode[numi].i_directos[0] = free_bit(&((private_data)->block_bitmap));
		struct directory_entry *directory = (struct directory_entry *) (private_data)->block[(private_data)->inode[numi].i_directos[0]- private_data->superblock->reserved_block];
		strcpy(directory[0].name, ".");
		directory[0].inode = &(private_data)->inode[numi];
		//Cambiar, el padre tiene que ser el inodo padre
		strcpy(directory[1].name, "..");
		directory[1].inode = &(private_data)->inode[numi];
		strcpy(directory[BLOCK_SIZE/sizeof(struct directory_entry)].name, "BASURA");
	}else if(type == 'f'){
		private_data->inode[numi].i_links = 1;
		private_data->inode[numi].i_tam = 0;
	}

}
void create_root(filesystem_t *private_data){
	unsigned long bloque;
	unsigned long numinode = free_bit(&(private_data->inode_bitmap));
	private_data->inode[numinode].i_num = numinode;
	private_data->inode[numinode].i_type = 'd';
	private_data->inode[numinode].i_tam = sizeof(struct directory_entry) * 2;
	private_data->inode[numinode].i_permisos = 666;
	private_data->inode[numinode].i_links = 2;
	memset(private_data->inode[numinode].i_directos, 0, sizeof(int)*N_DIRECTOS);
	//memcpy(inodo -> i_simple_ind, indirecto_simple, sizeof(int)*N_SIMPLES);
	private_data->inode[numinode].i_directos[0] = free_bit(&(private_data->block_bitmap));
	struct directory_entry *directory = (struct directory_entry *) private_data->block[private_data->inode[numinode].i_directos[0]- private_data->superblock->reserved_block];
	strcpy(directory[0].name, ".");
	directory[0].inode = &(private_data)->inode[numinode];
	strcpy(directory[1].name, "..");
	directory[1].inode = &(private_data)->inode[numinode];
	//Numero de entradas por bloque = BLOCK_SIZE/sizeof(struct directory_entry)
	strcpy(directory[BLOCK_SIZE/sizeof(struct directory_entry)].name, "BASURA");
}

