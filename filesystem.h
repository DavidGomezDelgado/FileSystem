#define N_DIRECTOS 10
//#define N_SIMPLES  1
//#define N_DOBLES 1
//#define N_TRIPLES 1

#define NUM_BLOCKS 1048576 //(4GB) = 1048576 * 4096
#define NUM_INODES 16777216 //1073741824÷64

#define BLOCK_SIZE 4096
#define MAGIC_N 123456


typedef struct directory_entry{
	char name[28];
	struct inode_fs *inode;
};



struct inode_fs{
	int i_num;
	char i_type;
	int i_tam;
	int i_permisos;
	int i_links;
	int i_directos[N_DIRECTOS]; //320 direcciones (32 bloques cada puntero)
	//offset en el fichero?
	char relleno[8]; //Bytes de relleno para tener un múltiplo del tamaño del bloque
};


//estructura que reutilizamos para bitmapb y bitmapi
typedef struct bitmap_t{
	unsigned char *array;
	uint64_t size;
};

//structura bloque de datos
//bloque con datos
typedef uint8_t block_t[BLOCK_SIZE];

typedef struct {
	struct superblock_fs *superblock;
	struct bitmap_t inode_bitmap;
	struct bitmap_t block_bitmap;
	struct inode_fs *inode;
	//array incompleto que nos dice en cuál bloque de datos estamos
	block_t *block;
	int fd;
} filesystem_t;


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
};


/***************************************************************
 * 							FUNCTIONS						   *
 ***************************************************************/
 
// bitmap.c
unsigned long free_bit(struct bitmap_t *);

// create_inode.c


// tree_manager.c
struct inode_fs *existe_inode(char *, struct directory_entry *);
struct inode_fs *inode_search (char *, struct inode_fs *, filesystem_t *);

// file_manager.c


// file_remove.c


// file_operations.c


// directory_operations.c


 
//create_inode.c
// struct inode_fs *create_inode(char, char*, struct inode_bitmap_fs *, struct block_bitmap_fs *);
// struct inode_fs *create_root(struct inode_bitmap_fs *, struct block_bitmap_fs *);
//
// //file_manager.c
// void touch (char *, char, char *, struct inode_fs *, struct inode_bitmap_fs *, struct block_bitmap_fs *);
// void print_directory(struct inode_fs);
// void clean_inode(struct inode_fs *, struct block_bitmap_fs *);
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
//
//
// //directory_operations.c
// char *read_directory(char *,struct inode_fs *, struct block_bitmap_fs *);
