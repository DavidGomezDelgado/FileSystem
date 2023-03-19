#define N_DIRECTOS 10
//#define N_SIMPLE  1
//#define N_DOBLES 1
//#define N_TRIPLES 1

#define NUM_BLOCKS 1000
#define NUM_INODES 1000


typedef struct {
	//int id_node;
	char name[32];
	struct Inode *file;
}directory_entry;

struct directory_table{//directo
		directory_entry directory_list[32];
};

struct _directory_table{ //indirecto simple
		directory_table directory_list[256];
};

struct __directory_table{//directo doble
		_directory_table directory_list[256];
};

struct ___directory_table{//directo triple
		__directory_table directory_list[256];
};

//Lista de bloques libres
typedef struct Block *Free_blocks_list; //Lista de bloques libres

//Descriptor de bloque
struct Block{
	//void devuelve un puntero a una dirección de memoria
	void *memory_address; //ID mejor?
	int block_size; //¿?
	Free_blocks_list next;
};

typedef struct Inode *Free_inodes_list;

struct Inode{
	char i_type;
	int i_tam;
	//int i_permission;,
	int i_directos[N_DIRECTOS];
	//int i_simple_ind[N_SIMPLES];
	//int i_doubles_ind[N_DOBLES];
	//int i_triple_ind[N_TRIPLES];
};

//superbloque
struct superblock{
	++int size;
	long free_blocks;
	Free_block_list free_block_list;
	struct Block next_free_block;
	long inode_list_size;
	Free_inode_list  free_inodes_list;
	struct Inode next_free_inode;
	unsigned char MODIFIED;
};

//BITMAP DE BLOQUES
struct block_bitmap{
	unsigned char bitmap[NUM_BLOCKS/8]; // Se divide entre 3 porque cada byte tiene 8 bits, por lo que sería un array de 125 bytes-
};

//BITMAP DE INODOS
struct inode_bitmap{
	unsigned char bitmap[NUM_INODES / 8];
};
