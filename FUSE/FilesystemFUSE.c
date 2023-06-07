#define FUSE_USE_VERSION 26
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <libgen.h>
#include "filesystem.h"


/**************************
 ---- GETATTR FUNCTION ----
 **************************/

/* stbuf: ptr a info del fichero */
static int fs_getattr (const char *path, struct stat *stbuf) {

	// El sistema llama a esta función cada vez que quiere obtener atributos de un archivo/fichero como permisos, quién es el usuario...

	printf("---- Entering mi_getattr function...\n");
	printf("---- fs_getattr - Path: %s\n", path);

	filesystem_t *private_data = (filesystem_t *) fuse_get_context() -> private_data;

	struct inode_fs *inode;
	int res = 0;
	char path_aux[70], base[70], dir[70];

	strcpy(path_aux, path);
	
//	strcpy(base, basename(path_aux));
//	strcpy(dir, dirname(path_aux));

	memset(stbuf, 0, sizeof(struct stat));
	
	// Inicializamos atributos de /
	if (strcmp(path, "/") == 0) {
		
		stbuf -> st_mode = S_IFDIR | private_data->inode[0].i_permisos;   // Directorio / con permisos 0777
		stbuf -> st_nlink = private_data->inode[0].i_links;

		stbuf -> st_uid = private_data -> st_uid;
		stbuf -> st_gid = private_data -> st_gid;

		stbuf -> st_atime = private_data -> st_atime;
		stbuf -> st_mtime = private_data -> st_mtime;
		stbuf -> st_ctime = private_data -> st_ctime;

		stbuf -> st_size = private_data->inode[0].i_tam;
		stbuf -> st_blocks = 8;

		printf("---- Attributes set successfully \\^o^/ !\n");

	} else {

		// Obtenemos el inodo del archivo o directorio
		inode = inode_search_path(path_aux, private_data);

		if (inode == NULL) {
			printf("---- fs_getattr - No entry... \"-.-\n");
			res = -ENOENT;
			return res;
		}

		if (inode->i_type == 'd') {
			stbuf -> st_mode = S_IFDIR | inode->i_permisos;   // Directorio con permisos 0777
		} else if (inode->i_type == 'f') {
			stbuf -> st_mode = S_IFREG | inode->i_permisos;   // Fichero con permisos 0666
		}

		stbuf -> st_nlink = inode->i_links;

		stbuf -> st_uid = private_data -> st_uid;
		stbuf -> st_gid = private_data -> st_gid;

		stbuf -> st_atime = private_data -> st_atime;
		stbuf -> st_mtime = private_data -> st_mtime;
		stbuf -> st_ctime = private_data -> st_ctime;

		stbuf -> st_size = inode->i_tam;
		stbuf -> st_blocks = 8;

	}
	
	printf("---- Attributes set successfully \\^o^/ !\n");

	return res;

}


/**************************
 ---- READDIR FUNCTION ----
 **************************/

/* path: path absoluto a nuestro punto de monataje, no desde raíz del PC sino desde raíz del punto de montaje */
static int fs_readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {

	// El sistema llama a esta función cuando queremos leer el directorio (cuando hacemos ls, ls -l, ls -la...).

	int i, j;
	struct directory_entry *entry;
	struct inode_fs *inode;
	int res = 0;
	char path_aux[70], base[70], dir[70];

	printf("---- Entering fs_readdir function...\n");

	filesystem_t *private_data = (filesystem_t *) fuse_get_context() -> private_data;

	strcpy(path_aux, path);

//	strcpy(base, basename(path_aux));
//	strcpy(dir, dirname(path_aux));

	if (strcmp(path, "/") == 0) {

		// Accedemos a sus entradas y las mostramos
		for (i = 0; i < N_DIRECTOS && (private_data->inode[0].i_directos[i] != 0); i++) {
			entry = (struct directory_entry *) private_data->block[private_data->inode[0].i_directos[i] - private_data->superblock->reserved_block];

			j = 0;
			while (j < max_entries && entry[j].inode != NULL) {
				/* Indicamos qué entradas deben estar en el path */
				if (filler(buf, entry[j].name, NULL, 0) != 0) {   // filler le dice al resto del sistema que existen tales entradas, NO LAS CREA
					break;
					return -ENOMEM;
				}
				j++;
			}
		}

	// Comprobamos si es directorio
	} else {

		// Obtenemos el inodo del archivo o directorio
		inode = inode_search_path(path_aux, private_data);
		
		if (inode == NULL) {
			printf("---- fs_getattr - No entry... \"-.-\n");
			res = -ENOENT;
		}

		// Accedemos a sus entradas y las mostramos
		for (i = 0; i < N_DIRECTOS && (private_data->inode[inode->i_num].i_directos[i] != 0); i++) {
			entry = (struct directory_entry *) private_data->block[private_data->inode[inode->i_num].i_directos[i] - private_data->superblock->reserved_block];

			j = 0;
			while (j < max_entries && entry[j].inode != NULL) {
				/* Indicamos qué entradas deben estar en el path */
				if (filler(buf, entry[j].name, NULL, 0) != 0) {   // filler le dice al resto del sistema que existen tales entradas, NO LAS CREA
					break;
					return -ENOMEM;
				}
				j++;

			}

		}

	}

	printf("---- Directory read successfully \\^o^/ !\n");

	return res;

}


/************************
 ---- MKDIR FUNCTION ----
 ************************/

static int fs_mkdir (const char *path, mode_t mode) {

	printf("---- Entering fs_mkdir function...\n");

	int res = 0;
	char path_aux[70], base[70], dir[70];

	filesystem_t *private_data = (filesystem_t *) fuse_get_context() -> private_data;
	
	strcpy(path_aux, path);

	// Obtenemos el archivo actual y el path hasta su padre
	strcpy(base, basename(path_aux));
	strcpy(dir, dirname(path_aux));

	if (make_dir(base, dir, private_data) == -1) {
		return -EEXIST;
	}

	printf("---- Directory created successfully \\^o^/ !\n");

	return res;

}


/************************
 ---- RMDIR FUNCTION ----
 ************************/

static int fs_rmdir (const char *path) {

	printf("---- Entering fs_mkdir function...\n");

	int res = 0;
	char path_aux[70], base[70], dir[70];

	filesystem_t *private_data = (filesystem_t *) fuse_get_context() -> private_data;

	strcpy(path_aux, path);

	// Obtenemos el archivo actual y el path hasta su padre
	strcpy(base, basename(path_aux));
	strcpy(dir, dirname(path_aux));
	
	if(rm_dir(base, dir, private_data) == -1){
		return -ENOENT;
	}else if (rm_dir(base,dir, private_data) == 1){
		return -ENOTEMPTY;
	}
	
	printf("---- Directory deleted succesfully \\ô^/ !\n");

	return res;

}


/*************************
 ---- UNLINK FUNCTION ----
 *************************/


/*************************
 ---- RENAME FUNCTION ----
 *************************/

static int fs_rename (const char *oldpath, const char *newpath/*, unsigned int flags*/) {

	printf("---- Entering fs_rename function...\n");

	int res = 0;
	struct inode_fs *inode;
	struct directort_entry *entry;
	char newpath_aux[70], oldpath_aux[70], base[70], dir[70];

	filesystem_t *private_data = (filesystem_t *) fuse_get_context() -> private_data;

	strcpy(oldpath_aux, oldpath);
	strcpy(newpath_aux, newpath);

	// Obtenemos el archivo actual y el path hasta su padre
	strcpy(base, basename(newpath_aux));
	strcpy(dir, dirname(newpath_aux));

	if (rename_file(oldpath_aux, base, private_data) == -1) {
		return -ENOENT;
	}

	printf("---- Directory or file renamed successfully \\^o^/ !\n");

	return res;

}


/***********************
 ---- OPEN FUNCTION ----
 ***********************/

//fs_open (char * path/nombrefichero,  struct fuse_file_info * fi)
/*
static int open (const char *path, struct fuse_file_info *fi) {

	// El sistema llama a esta función cuando se abre un fichero para lectura o escritura

	printf("---- Entering fs_open function...\n");

	int res = 0, i, j = 0, encontrado = 0;
	struct directory_entry *entry;
	struct inode_fs *inode;
	char path_aux[70], base[70], dir[70];

	filesystem_t *private_data = (filesystem_t *) fuse_get_context() -> private_data;

	strcpy(path_aux, path);

	// Obtenemos el archivo actual y el path hasta su padre
	strcpy(base, basename(path_aux));
	strcpy(dir, dirname(path_aux));

	if (strcmp(path, "/") == 0) {

		// Recorremos sus entradas buscando ficheros
		for (i = 0; i < N_DIRECTOS && (private_data->inode[0].i_directos[i] != 0) && !encontrado; i++) {
			entry = (struct directory_entry *) private_data->block[private_data->inode[0].i_directos[i] - private_data->superblock->reserved_block];

			while (j < max_entries && entry[j].inode != NULL && !encontrado) {
				// Comprobamos que la entrada coincida con el nombre y que sea fichero
				if (strcmp(entry[j].name, base) == 0 && entry[j].inode->i_type == 'f') {
					// Guardamos el índice de inodo para no tener que buscarlo en read
					fi -> fh = entry[j].inode->i_num;
					encontrado = 1;
					printf("---- inodo: %d  fi -> fh: %lu\n", private_data->inode[entry[j].inode->i_num].i_num, fi -> fh);
				}
				j++;
			}
		}

		if (!encontrado) {
			printf("---- mi_open - No entry... \"-.- \n");
			return -ENOENT;
		}

	} else {
		// Obtenemos inodo del fichero
		inode = inode_search_path(path_aux, private_data);

		if (inode == NULL) {
			return -ENOENT;
		}

		// Guardamos el índice de inodo para no tener que buscarlo en read
		fi -> fh = inode->i_num;
		printf("---- inodo: %d  fi -> fh: %lu\n", private_data->inode[inode->i_num].i_num, fi -> fh);

	}



	// Comprobamos si tenemos acceso a lectura y escritura
	if ((fi -> flags & 3) != O_RDONLY) {   // los 3 lsb son los modos de open (append, read only,...)
		return -EACCES;
	}

	printf("---- File opened successfully \\^o^/ !\n");

	return res;

}
*/

/***********************
 ---- READ FUNCTION ----
 ***********************/
 

/************************
 ---- WRITE FUNCTION ----
 ************************/
 

/*************************
 ---- FUSE OPERATIONS ----
 *************************/

static struct fuse_operations basic_oper = {
	.getattr	= fs_getattr,   
	.readdir	= fs_readdir,
	//.open		= fs_open,
	//.read		= fs_read,
	//.write	= fs_write,
	.rmdir		= fs_rmdir,
	.mkdir		= fs_mkdir,
	//.unlink		= fs_unlink,
	.rename		= fs_rename,
};

/**********************
 ---- MAIN PROGRAM ----
 **********************/

int main (int argc, char *argv[]) {
	printf("Aqui empieza\n");
	filesystem_t *private_data = malloc(sizeof(filesystem_t));
	int file;
	struct stat fileStat;
	
	if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-')) error_parametros();
	printf("%s\n", argv[2]);
	
	// NOS DA ERROR INVALID ARGUMENT: punto_montaje
	printf("%s\n", argv[1]);
	file = open(argv[1], O_RDWR, 0666);
	//file = open("filesystem.img", O_RDWR, 0666);
	if (file == -1) {
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}
	
	if(fstat(file, &fileStat) == -1){
		perror("stat");
		exit(EXIT_FAILURE);
	}

	argv[argc-2] = argv[argc-1];
	argv[argc-1] = NULL;
	argc--;
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
	printf("Aqui termina\n");
	return fuse_main(argc, argv, &basic_oper, private_data);

}
