#define N_DIRECTOS 10
//#define N_SIMPLES  1
//#define N_DOBLES 1
//#define N_TRIPLES 1

#define NUM_BLOCKS 1000
#define NUM_INODES 1000

#define BLOCK_SIZE 1024


typedef struct directory_entry{
	char name[24];
	struct inode_fs *inode;
};



struct inode_fs{
	int i_num;
	char i_type;
	int i_tam;
	int i_permisos;
	int i_directos[N_DIRECTOS]; //320 direcciones (32 bloques cada puntero)
};



//BITMAP DE BLOQUES
struct block_bitmap_fs{
	unsigned char bitmap[NUM_BLOCKS/8]; // Se divide entre 8 porque cada byte tiene 8 bits, por lo que sería un array de 125 bytes-
	uintptr_t map[NUM_BLOCKS];
}; 

//BITMAP DE INODOS
struct inode_bitmap_fs{
	unsigned char bitmap[NUM_INODES / 8];
};


/***************************************************************
 * 							FUNCTIONS						   *
 ***************************************************************/
 
 //bitmap.c
 int free_inode(struct inode_bitmap_fs *);
 int free_block(struct block_bitmap_fs *);
 
 //create_inode.c
 struct inode_fs *create_inode(char, char*, struct inode_bitmap_fs *, struct block_bitmap_fs *);
 struct inode_fs *create_root(struct inode_bitmap_fs *, struct block_bitmap_fs *);
 
 //file_manager.c
 void touch (char *, char, char *, struct inode_fs *, struct inode_bitmap_fs *, struct block_bitmap_fs *);
 void print_directory(struct inode_fs);
 void clean_inode(struct inode_fs *, struct block_bitmap_fs *);
 
 //tree_manager.c
 struct inode_fs *inode_search(char *, struct inode_fs *, struct block_bitmap_fs *);
 struct inode_fs *_inode_search(char *, char *, struct inode_fs, struct block_bitmap_fs);

 //file_remove.c
 void rm (char *, struct inode_fs *, struct inode_bitmap_fs *, struct block_bitmap_fs *);

 //file_operations.c
 void file_edit(char *, char *, struct inode_fs *, struct block_bitmap_fs *);
 char *read_file(char *,struct inode_fs *, struct block_bitmap_fs *);
 void rename_file(char *, char *, struct inode_fs *, struct block_bitmap_fs* );


 //directory_operations.c
 char *read_directory(char *,struct inode_fs *, struct block_bitmap_fs *);
