#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define N_DIRECTOS 10
//#define N_SIMPLES  1
//#define N_DOBLES 1
//#define N_TRIPLES 1

#define NUM_BLOCKS 1048576 //(4GB) = 1048576 * 4096
#define NUM_INODES 16777216 //1073741824÷64

#define BLOCK_SIZE 4096
#define MAGIC_N 123456

#define max_entries (BLOCK_SIZE / sizeof(struct directory_entry *))


struct directory_entry{
	char name[28];
	struct inode_fs *inode;
};

struct inode_fs{
	int i_num;
	char i_type;
	int i_tam;
	int i_permisos;
	int i_links;
	int i_directos[N_DIRECTOS]; //1280 direcciones (128 bloques cada puntero)
	//offset en el fichero?
	char relleno[8]; //Bytes de relleno para tener un múltiplo del tamaño del bloque
};


// Estructura que reutilizamos para bitmapb y bitmapi
struct bitmap_t{
	unsigned char *array;
	uint64_t size;
};

// Estructura bloque de datos
// bloque con datos
typedef uint8_t block_t[BLOCK_SIZE];

struct superblock_fs {
	unsigned long magic_number;
	unsigned long bitmapb_offset;
	unsigned long bitmapi_offset;
	unsigned long free_blocks;
	unsigned long free_inodes;
    unsigned long num_blocks;
    unsigned long num_inodes;
    unsigned long offset_inodos;
    unsigned long offset_bloques;
    unsigned long block_size;
    unsigned long inode_size;
    unsigned long reserved_block;
};

typedef struct {
	struct superblock_fs *superblock;
	struct bitmap_t inode_bitmap;
	struct bitmap_t block_bitmap;
	struct inode_fs *inode;
	block_t *block;   //array incompleto que nos dice en cuál bloque de datos estamos
	int fd;
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

// directory_manager.c
void make_dir (char *, char *, struct inode_fs *, filesystem_t *);
// void rm_dir (char *, struct inode_fs *, filesystem_t *);

// file_manager.c
void update_entry (char *, struct inode_fs *, struct inode_fs *, filesystem_t *);
void touch (char *, char *, struct inode_fs *, filesystem_t *);
void clean_inode (struct inode_fs *, filesystem_t *);
// void rename_file(char *, char *, struct inode_fs *, struct block_bitmap_fs* );

// file_remove.c
struct directory_entry *search_last_entry (struct inode_fs *, filesystem_t *);  // CAUTION !!!
void remove_dentry (char *, struct inode_fs *, filesystem_t *);
void remove_inode (struct inode_fs *, filesystem_t *);
void rm (char *, struct inode_fs *, filesystem_t *);

// file_operations.c
// void file_edit(char *, char *, struct inode_fs *, struct block_bitmap_fs *);
// char *read_file(char *,struct inode_fs *, struct block_bitmap_fs *);

// directory_operations.c
void read_directory(char *, struct inode_fs *, filesystem_t *);

//
// //file_manager.c
// void touch (char *, char, char *, struct inode_fs *, struct inode_bitmap_fs *, struct block_bitmap_fs *);
// void print_directory(struct inode_fs);
// void clean_inode(struct inode_fs *, struct block_bitmap_fs *);
// void rename_file(char *, char *, struct inode_fs *, struct block_bitmap_fs* );
//
// //tree_manager.c
// struct inode_fs *inode_search(char *, struct inode_fs *, struct block_bitmap_fs *);
// struct inode_fs *_inode_search(char *, char *, struct inode_fs, struct block_bitmap_fs);
//
// //file_remove.c
// void rm (char *, struct inode_fs *, struct inode_bitmap_fs *, struct block_bitmap_fs *);
//
// //file_operations.c
// void file_edit(char *, char *, struct inode_fs *, struct block_bitmap_fs *);
// char *read_file(char *,struct inode_fs *, struct block_bitmap_fs *);
//
// //directory_operations.c
// char *read_directory(char *,struct inode_fs *, struct block_bitmap_fs *);

#endif
