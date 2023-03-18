#define N_DIRECTOS 10;
#define N_SIMPLE  1;
//#define N_DOBLES 1:
//#define N_TRIPLES 1;

typedef char Name[32];
typedef struct file_register{
	int id_node;
	Name f_nombre;
};

//Lista de bloques libres
typedef struct Block *Free_blocks_list;

struct Block{
	//void devuelve un puntero a una dirección de memoria
	void *memory_address; //ID mejor?
	int block_size; //¿?
	Free_blocks_list next;
	
	file_register files[32];
};

typedef struct Inode *Free_inodes_list;

struct Inode{
	char i_type;
	int i_tam;
	//int i_permission;,
	int i_directos[N_DIRECTOS];
	//int i_doubles_ind[N_DOBLES];
	//int i_triple_ind[N_TRIPLES];
};

//superbloque
struct superblock{
	++int size;
	long free_blocks;
	Free_block_list free_block_list;
	Block next_free_block;
	long inode_list_size;
	Free_inode_list  free_inodes_list;
	Inode next_free_inode;
	unsigned char MODIFIED;
};
