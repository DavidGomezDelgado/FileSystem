#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define N_DIRECTOS 10
//#define N_SIMPLES  1
//#define N_DOBLES 1
//#define N_TRIPLES 1

#define BLOCK_SIZE 4096
#define MAGIC_N 123456

#define max_entries (BLOCK_SIZE / sizeof(struct directory_entry *))

/* Estructura de entrada de directorio */
struct directory_entry{
	char name[28];
	long inode;
};

/* Estructura de inodo */
struct inode_fs{
	long i_num;
	char i_type;
	int i_tam;
	int i_permisos;
	int i_links;
	int i_directos[N_DIRECTOS]; //1280 direcciones (128 bloques cada puntero)
	//offset en el fichero?
};


/* Estructura de bitmap, tanto para inodos como para bloques */
struct bitmap_t{
	unsigned char *array;
	unsigned long long size;
};

/* Estructura de bloque de datos */
typedef unsigned char block_t[BLOCK_SIZE];

/* Estructura de superbloque con metadatos */
struct superblock_fs {
	unsigned long magic_number;
	unsigned long bitmapb_offset;
	unsigned long bitmapi_offset;
	//unsigned long free_blocks;
	unsigned long inodes_ocupados;
    unsigned long num_blocks;
    unsigned long num_inodes;
    unsigned long offset_inodos;
    unsigned long offset_bloques;
    unsigned long block_size;
    unsigned long inode_size;
    unsigned long reserved_block;
};

/* Estructura de nuestro sistema de ficheros */
typedef struct {
	struct superblock_fs *superblock;
	struct bitmap_t inode_bitmap;
	struct bitmap_t block_bitmap;
	struct inode_fs *inode;
	block_t *block;   //array incompleto que nos dice en cuál bloque de datos estamos
	struct timespec st_atim;  				/* fechas del fichero */
	struct timespec st_mtim;
	struct timespec st_ctim;
	uid_t     st_uid;        				/* El usuario y grupo */
	gid_t     st_gid;
} filesystem_t;


/***************************************************************
 * 							FUNCTIONS						   *
 ***************************************************************/
 
 //bitmap.c
unsigned long free_bit(struct bitmap_t *);

 //create_inode.c
struct inode_fs *create_inode(char, filesystem_t *);
struct inode_fs *create_root(filesystem_t *);

// tree_manager.c
struct inode_fs *existe_inode(char *, struct directory_entry *);
struct inode_fs *inode_search (char *, struct inode_fs *, filesystem_t *);
struct inode_fs *inode_search_path(char *, filesystem_t *);

// directory_manager.c
int make_dir (char *, char *, filesystem_t *);
int rm_dir (char *, char *, filesystem_t *);

// file_manager.c
void update_entry (char *, struct inode_fs *, struct inode_fs *, filesystem_t *);
int touch (char *, char *, filesystem_t *);
void clean_inode (struct inode_fs *, filesystem_t *);
int rename_file(char *, char *, filesystem_t *);

// file_remove.c
struct directory_entry *search_last_entry (struct inode_fs *, filesystem_t *);  // CAUTION !!!
void remove_dentry (char *, struct inode_fs *, filesystem_t *);
void remove_inode (struct inode_fs *, filesystem_t *);
int rm (char *, filesystem_t *);

// file_operations.c
size_t file_edit(const char *, char *, size_t, off_t,filesystem_t *);
char *read_file(char *, struct inode_fs*, filesystem_t *);

// directory_operations.c
void read_directory(char *, filesystem_t *);

// error.c
void error_parametros();

#endif
