#define FUSE_USE_VERSION 26
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "filesystem.h"

void set_fields (struct inode_fs *inode, struct stat *stbuf, filesystem_t *private_data) {
	
	int j, i = 0;
	struct directory_entry *entry;
	
	// Inicializamos atributos
	stbuf -> st_nlink = private_data->inode[inode->i_num].i_links;   
	
	stbuf -> st_uid = private_data -> st_uid;
	stbuf -> st_gid = private_data -> st_gid;
	
	stbuf -> st_atime = private_data -> st_atime;
	stbuf -> st_mtime = private_data -> st_mtime;
	stbuf -> st_ctime = private_data -> st_ctime;
	
	stbuf -> st_size = private_data->inode[inode->i_num].i_tam;
	
	if (inode->i_type == 'd') {
		stbuf -> st_mode = S_IFDIR | inode->i_permisos;   // Directorio de lectura
		stbuf -> st_blocks = 8;
	
		// Recorremos punteros directos del directorio
		while (i < N_DIRECTOS && (inode->i_directos[i] != 0)) {

			// Obtenemos entradas del bloque apuntado por el puntero directo
			entry = (struct directory_entry *) private_data->block[inode->i_directos[i] - private_data->superblock->reserved_block];

			// Recorremos entradas
			for (j = 0; j < 128 && (entry[j].inode != NULL); j++) {
				if (strcmp(entry[j].name, ".") != 0 || strcmp(entry[j].name, "..") != 0) {
					set_fields (entry[j].inode, stbuf, private_data);
				}

				i++;

			}

		}

	} else {
		stbuf -> st_mode = S_IFREG | inode->i_permisos;   // Fichero regular
		// stbuf -> st_blocks = 8;  // DUDA
	}

}

/* stbuf: ptr a info del fichero */
static int fs_getattr (const char *path, struct stat *stbuf) {

	filesystem_t *private_data = (filesystem_t *) fuse_get_context() -> private_data;
	struct directory_entry *entry;
	int res = 0;
	int i, j;
	
	memset(stbuf, 0, sizeof(struct stat));
	
	// Inicializamos atributos de /
	stbuf -> st_mode = S_IFDIR | private_data->inode[0].i_permisos;   // Directorio de lectura y ejecución para group y other, y todo para user
	stbuf -> st_nlink = private_data->inode[0].i_links;
	
	stbuf -> st_uid = private_data -> st_uid;
	stbuf -> st_gid = private_data -> st_gid;
	
	stbuf -> st_atime = private_data -> st_atime;
	stbuf -> st_mtime = private_data -> st_mtime;
	stbuf -> st_ctime = private_data -> st_ctime;
	
	stbuf -> st_size = private_data->inode[0].i_tam;
	stbuf -> st_blocks = 8;
	
	// Recorremos punteros directos del directorio
	while (i < N_DIRECTOS && (private_data->inode[0]->i_directos[i] != 0)) {

		// Obtenemos entradas del bloque apuntado por el puntero directo
		entry = (struct directory_entry *) private_data->block[private_data->inode[0]->i_directos[i] - private_data->superblock->reserved_block];

		// Recorremos entradas
		for (j = 0; j < 128 && (entry[j].inode != NULL); j++) {
			if (strcmp(entry[j].name, ".") != 0 || strcmp(entry[j].name, "..") != 0) {
				set_fields (entry[j].inode, stbuf, private_data);
			}

			i++;

		}

	}
	
	printf("---- Attributes set successfully \\^o^/ !\n");
	
	return res;

}

/*************************
 ---- FUSE OPERATIONS ----
 *************************/

static struct fuse_operations basic_oper = {
	.getattr	= fs_getattr,   
	//.readdir	= fs_readdir,
	//.open		= fs_open,
	//.read		= fs_read,
	//.mknod		= fs_mknod,
	//.mkdir		= fs_mkdir,
	//.unlink		= fs_unlink,
};

/**********************
 ---- MAIN PROGRAM ----
 **********************/

int main (int argc, char *argv[]) {

	filesystem_t *private_data = malloc(sizeof(filesystem_t));
	int file;
	struct stat fileStat;
	
	if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-')) error_parametros();
	
	file = open("filesystem.img", O_RDWR);
	if (file == -1) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}
	
	if(fstat(file, &fileStat) == -1){
		perror("stat");
		exit(EXIT_FAILURE);
	}

	// Inicializamos estructura	
	private_data->superblock = mmap(NULL, sizeof(struct superblock_fs), PROT_WRITE|PROT_READ, MAP_SHARED,file, 0);
	private_data->block_bitmap.size = private_data->superblock->num_blocks/8;
	private_data->block_bitmap.array = mmap(NULL, private_data->block_bitmap.size, PROT_WRITE|PROT_READ, MAP_SHARED,file, private_data->superblock->bitmapb_offset);
	private_data->inode_bitmap.size = private_data->superblock->num_inodes/8;
	private_data->inode_bitmap.array = mmap(NULL, private_data->inode_bitmap.size, PROT_WRITE|PROT_READ, MAP_SHARED,file, private_data->superblock->bitmapi_offset);

	private_data->inode = mmap(NULL,private_data->superblock->bitmapi_offset - private_data->superblock->bitmapb_offset, PROT_WRITE|PROT_READ, MAP_SHARED,file, private_data->superblock->offset_inodos);
	private_data->block = (block_t*) mmap(NULL,fileStat.st_size-private_data->superblock->offset_bloques, PROT_WRITE|PROT_READ, MAP_SHARED,file, private_data->superblock->offset_bloques);
	private_data->fd = file;
	
	private_data -> st_uid = fileStat.st_uid;
	private_data -> st_gid = fileStat.st_gid;

	private_data -> st_atime = fileStat.st_atime;
	private_data -> st_ctime = fileStat.st_ctime;
	private_data -> st_mtime = fileStat.st_mtime;

	close(file);
	
	return fuse_main(argc, argv, &basic_oper, private_data);

}
