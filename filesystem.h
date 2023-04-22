#define N_DIRECTOS 10
#define N_SIMPLES  1
//#define N_DOBLES 1
//#define N_TRIPLES 1

#define NUM_BLOCKS 1000
#define NUM_INODES 1000


typedef struct {
	char name[24];
	struct inode *inode;
} directory_entry;

//Lista de bloques libres
typedef struct block *free_blocks_list; //Lista de bloques libres

//Descriptor de bloque
struct block{
	//void devuelve un puntero a una dirección de memoria
	void *memory_address;
	int block_size;
	free_blocks_list next;
};

// Lista de inodos libres
typedef struct inode *free_inodes_list;

struct inode_fs{
	int i_num;
	char i_name[24];
	char i_type;
	int i_tam;
	//int i_permission;
	int i_directos[N_DIRECTOS]; //320 direcciones
	int i_simple_ind[N_SIMPLES];
	//int i_doubles_ind[N_DOBLES];
	//int i_triple_ind[N_TRIPLES];
	//char i_relleno[20];
};

//superbloque
struct superblock{
	long free_blocks;
	free_blocks_list free_blocks_list;
	long inode_list_size;
	free_inodes_list  free_inodes_list;
	unsigned char MODIFIED;
};

//BITMAP DE BLOQUES
struct block_bitmap_fs{
	unsigned char bitmap[NUM_BLOCKS/8]; // Se divide entre 3 porque cada byte tiene 8 bits, por lo que sería un array de 125 bytes-
}; 

//BITMAP DE INODOS
struct inode_bitmap_fs{
	unsigned char bitmap[NUM_INODES / 8];
};

/*********************************************************************************************
 * 							FUNCTIONS									
 * ******************************************************************************************/
 
 //bitmap.c
 int free_inode(struct inode_bitmap_fs *);
 
 //create_inode.c
 struct inode_fs *create_inode(char , char *, struct inode_bitmap_fs *);
 struct inode_fs *create_root(struct inode_bitmap_fs *);
 
 //file_manager.c
 void touch (char *, char, char *, struct inode_fs *, struct inode_bitmap_fs *);
 void print_directory(struct inode_fs);
 
 //tree_manager.c
 struct inode_fs *inode_search(char *, struct inode_fs *);
 void insert(char *, struct inode_fs *, struct inode_fs);



