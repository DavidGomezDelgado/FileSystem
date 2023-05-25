#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include "filesystem.h"

#define MAGIC_N 123456

struct superblock_fs {
	unsigned long magic_number;
	unsigned long bitmapb_offset;
	unsigned long bitmapi_offset;
    unsigned long num_blocks;
    unsigned long num_inodes;
    unsigned long block_size;
    char relleno [BLOCK_SIZE-40];
};

int main(int argc, char *argv[]){
	int file, nBloquesReservados = 1;
	struct stat statbuf;
	struct superblock_fs *superblock;
	struct block_bitmap_fs *bitmapb;
	struct inode_bitmap_fs *bitmapi;
	printf("%d\n", sizeof(struct superblock_fs));

	int pagesize = sysconf(_SC_PAGE_SIZE);

	file = open("filesystem.img", O_RDWR);
	if (file == -1) {
	   perror("open");
	   exit(EXIT_FAILURE);
	}
	if (fstat(file,&statbuf) == -1){
		perror("stat");
		exit(EXIT_FAILURE);
	}
	/*
	statbuf.st_size
	statbuf.st_blksize
	statbuf.st_blocks
    */
	

	superblock = mmap(NULL,BLOCK_SIZE,PROT_WRITE|PROT_READ,MAP_SHARED,file, 0);
	 if (superblock == MAP_FAILED) {
		 perror("Error al mapear el archivo");
		 close(file);
		 exit( EXIT_FAILURE);
	 }
	 superblock->magic_number = MAGIC_N;
	 superblock->bitmapb_offset = BLOCK_SIZE; /* calcular el tamaño del bitmap dependiendo del tamaño del fichero */
	 superblock->block_size = BLOCK_SIZE;
	 superblock->num_blocks = NUM_BLOCKS;
	 superblock->num_inodes = NUM_INODES;
	 printf("%lu\n,%lu\n,%lu\n,%lu\n, %lu\n",superblock->magic_number, superblock->num_blocks, superblock->num_inodes, superblock->block_size, superblock->bitmapb_offset);


	 //movemos el puntero del fichero a donde apunta el offset
	 unsigned long bitmap_offset = superblock->bitmapb_offset;
//	 lseek(file, bitmap_offset, SEEK_SET);
	 int size_bitmap = sizeof(bitmapb->bitmap);

	 if(BLOCK_SIZE < size_bitmap){
		 nBloquesReservados = size_bitmap/BLOCK_SIZE;
		 if (!(size_bitmap % BLOCK_SIZE)) nBloquesReservados++;
	 }
	 /* tamaño de pagina (?) */

//
	 bitmapb = mmap(NULL,BLOCK_SIZE*nBloquesReservados,PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->bitmapb_offset);
	 // close(file);
	 // ... siguen existiendo los mapeos en memoria del fichero
	 
	 
	 /*
	 typedef char[][BLOCKSIZE] block_t;
	 
	 typedef {
	    char *array;
	    uint64_t size;
     } bitmap_t;
     
	 typedef {
	    struct superblock_t *sb;
	    struct bitmap_t blockBM;
	    struct bitmap_t inodeBM;
	    struct inode_t *inode;
	    struct block_t *block;
     } filesystem_t;
     
     filesystem_t myfilesystem;
     
     myfilesystem.sb->numBLOCK = XXX 
     
     int fd = open(argv[1],O_RDWR);
     fstat(fd, &statbuff);
          
     myfilesystem.inodeBM.array= (unsigned char*)mmap(....);
	 myfilesystem.inodeBM.size = ...; // numero de elementos / 8;
	 ....
	 mmap
	 ....
	 close(fd);
	 ...
	 
	 elem = getItem(myfilesystem.blockBM); // check -1
	 freeItem(&mybitmap, elem);
     
     main_fuse(......., &myfilesystem);
     */  
     /* 
     dentro de fuse
     struct filesystem_t *private "<- &myfilesystem"
     
     
	 private->inodeBM.array= (unsigned char*)mmap(....);
	 private->inodeBM.size = ...; // numero de elementos / 8;
	 
	 write(..buf.offset..count.)
	 {
	   block_num = offset / BLOCKSIZE;
	   suboffset = offset % BLOCKSIZE;
	   myblocknumber = private->inode[ith].i_directos[block_num]; // ultimo
	   private->block[myblocknumber][suboffset] = buf; // memcpy(...)
     }
	 
	 elem = getItem(myfilesystem.blockBM); // check -1
	 freeItem(&mybitmap, elem);
	 
	 int freeItem(bitmap_t *bitmap, long n)
	 {
	    uint64_t i = n/8; // n >> 3
	    unsigned char j = n%8; // n & 0x07
	    unsigned char mask = (0x01 << j);
	    if (! (bitmap->array[i] & mask)) {
	       // errno = EINVAL;
	       return -1;
	    }
	    bitmap->array[i] &= ~mask;
	    return 0;
     }
     int64_t getItem(bitmap_t *bitmap)
     {
        uint64_t i;
        char *p;
        for (i=0, p=bitmap->array; i<bitmap->size; i++, p++) {
           if (*p != 0xff) break;
	    }
	    if (i < bitmap->size) {
	        for (j = 0; j < 8; j++) {
	            
		    }
	    } else {
	        errno = ENOSPC;
	        return -1;
	    }
     }
	 */
	 
	 printf("%d\n", sizeof(bitmapb->bitmap));

	 superblock->bitmapi_offset = BLOCK_SIZE*nBloquesReservados + superblock->bitmapb_offset;

	 off_t bitmapi_offset = superblock->bitmapi_offset;
	 lseek(file, bitmapi_offset, SEEK_SET);

	 if(BLOCK_SIZE < sizeof(bitmapi->bitmap)){
	 		 nBloquesReservados = sizeof(bitmapi->bitmap)/BLOCK_SIZE;
	 }

	 bitmapi = mmap(NULL,BLOCK_SIZE*nBloquesReservados,PROT_WRITE|PROT_READ, MAP_SHARED,file, superblock->bitmapi_offset);
	 printf("%d\n", sizeof(bitmapi->bitmap));

	printf("Todo guardado\n");
	//msync(superblock, BLOCK_SIZE, MS_SYNC);
	//munmap(superblock, BLOCK_SIZE);
	close(file);

	file = open("filesystem.img", O_RDONLY);
	//superblock = mmap(NULL,BLOCK_SIZE,PROT_WRITE|PROT_READ,MAP_SHARED,file, 0);
	 lseek(file, 0, SEEK_SET);
	printf("%lu\n,%lu\n,%lu\n,%lu\n, %lu\n, %lu\n",superblock->magic_number, superblock->num_blocks, superblock->num_inodes, superblock->block_size, superblock->bitmapb_offset, superblock->bitmapi_offset);
	//munmap(superblock, BLOCK_SIZE);
	close(file);
}





/*#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../filesystem.h"

#define NUM_BLOCKS 1048576 //(4GB)
#define NUM_INODES 1000

#define BLOCK_SIZE 4096

struct superblock_fs {
    int num_blocks;
    int num_inodes;
    int block_size;
    struct block_bitmap_fs block_bitmap;
    struct inode_bitmap_fs inode_bitmap;
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Debe proporcionar el nombre del archivo para almacenar el superbloque.\n");
		return 1;
	}
	
	// Inicializar el superbloque
	struct superblock_fs superblock;
	memset(&superblock, 0, sizeof(struct superblock_fs));
	superblock.num_blocks = NUM_BLOCKS;
	superblock.num_inodes = NUM_INODES;
	superblock.block_size = BLOCK_SIZE;

	// Inicializar el mapa de bits de bloques y de inodos
	memset(&superblock.block_bitmap, 0, sizeof(struct block_bitmap_fs));
	memset(&superblock.inode_bitmap, 0, sizeof(struct inode_bitmap_fs));

	// Crear el inodo raíz
	struct inode_fs *root_inode = create_root(&superblock.inode_bitmap, &superblock.block_bitmap);

	// Escribir el superbloque en un archivo o dispositivo
	FILE *file = fopen(argv[1], "w");
	if (file == NULL) {
    		printf("Error al abrir el archivo.\n");
    		return 1;
	}

	fprintf(file, "%d\n", superblock.num_blocks);
	fprintf(file, "%d\n", superblock.num_inodes);
	fprintf(file, "%d\n", superblock.block_size);

	for (int i = 0; i < NUM_BLOCKS / 8; i++) {
	    fprintf(file, "%02X ", superblock.block_bitmap.bitmap[i]);
	    fprintf(file, "\n");
	}

	for (int i = 0; i < NUM_INODES / 8; i++) {
	    fprintf(file, "%02X ", superblock.inode_bitmap.bitmap[i]);
	    fprintf(file, "\n");
	}

	fclose(file);

	printf("Superbloque inicializado correctamente.\n");

	return 0;
}*/
