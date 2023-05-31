#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include "filesystem.h"

int main (int argc, char *argv[])
{
	fflush(stdout);

	int pagesize = sysconf(_SC_PAGE_SIZE);  // Obtenemos tamaño de página del sistema
	struct stat statbuf;
	myfilesystem = malloc(sizeof(struct filesystem_t));
	int file;

	/* Abrimos el fichero para el filesystem */
//	file = open(argv[1], O_RDWR);
	file = open("filesystem.img", O_RDWR);
	if (file == -1) {
		perror("open :[");
		exit(EXIT_FAILURE);
	}

	/* Obtenemos datos del fichero */
	if (fstat(file, &statbuf)) {
		perror("fstat :[");
		exit(EXIT_FAILURE);
	}

	printf("statbuf.stsize: %ld\n", statbuf.st_size);

	/* Datos interesantes
	 * statbuf.st_size;      // Total size, in bytes
	 * statbuf.st_blksize;   // Block size for filesystem I/O
	 * statbuf.st_blocks;    // Number of 512B blocks allocated
	 */

	printf("Todo listo para mapear \\^o^/ ! \n");

	/*****************************
	 * Inicializamos superbloque *
	 *****************************/

	/* Mapeamos espacio para el superbloque en el fichero */
	myfilesystem->sb = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
	if (myfilesystem->sb == MAP_FAILED) {
		perror("superblock mmap :[");
		close(file);
		exit( EXIT_FAILURE);
	}

	myfilesystem->sb->magic_number = MAGIC_N;
	myfilesystem->sb->block_size = BLOCK_SIZE;
	myfilesystem->sb->num_blocks = statbuf.st_size / BLOCK_SIZE;
	printf("num_blocks: %lu\n", myfilesystem->sb->num_blocks);
	myfilesystem->sb->num_inodes = (statbuf.st_size / 4) / sizeof(struct inode_fs);   // 1/4 del espacio para inodos  ¿¿DUDA: Inodos con tam de bloque o de inode_fs??
	printf("num_inodes: %lu\n", myfilesystem->sb->num_inodes);

	/**************************************
	 * ¡¡¡ COMPROBAR BIEN LOS OFFSETS !!! *
	 **************************************/
	/* Bitmap de bloques */
	myfilesystem->sb->bitmapb_offset = 1;   // 1 porque vamos de bloque en bloque (luego multiplicamos por 4096
	if (((myfilesystem->sb->num_blocks / 8) % BLOCK_SIZE) == 0) {
		myfilesystem->sb->bitmapb_reserved_blocks = (myfilesystem->sb->num_blocks / 8) / BLOCK_SIZE;   // num_blocks / 8 porque 1 Byte del bitmap representa 8 bloques
	} else {
		myfilesystem->sb->bitmapb_reserved_blocks = (myfilesystem->sb->num_blocks / 8) / BLOCK_SIZE + 1;   // num_blocks / 8 porque 1 Byte del bitmap representa 8 bloques
	}
	printf("bitmapb_reserved_blocks: %lu\n", myfilesystem->sb->bitmapb_reserved_blocks);

	/* Bitmap de inodos */
	myfilesystem->sb->bitmapi_offset = myfilesystem->sb->bitmapb_offset + myfilesystem->sb->bitmapb_reserved_blocks;
	printf("bitmapi_offset: %lu\n", myfilesystem->sb->bitmapi_offset);

	if (((myfilesystem->sb->num_inodes / 8) % BLOCK_SIZE) == 0) {
		myfilesystem->sb->bitmapi_reserved_blocks = (myfilesystem->sb->num_inodes / 8) / BLOCK_SIZE;
	} else {
		myfilesystem->sb->bitmapi_reserved_blocks = (myfilesystem->sb->num_inodes / 8) / BLOCK_SIZE + 1;
	}
	printf("bitmapi_reserved_blocks: %lu\n", myfilesystem->sb->bitmapi_reserved_blocks);

	/* Inodos */
	myfilesystem->sb->offset_inodos = myfilesystem->sb->bitmapi_offset + myfilesystem->sb->bitmapi_reserved_blocks;
	printf("offset_inodos: %lu\n", myfilesystem->sb->offset_inodos);

	if ((myfilesystem->sb->num_blocks % 4) == 0) {
		myfilesystem->sb->inode_reserved_blocks = myfilesystem->sb->num_blocks / 4;
	} else {
		myfilesystem->sb->inode_reserved_blocks = myfilesystem->sb->num_blocks / 4 + 1;
	}
	printf("inode_reserved_blocks: %lu\n", myfilesystem->sb->inode_reserved_blocks);

	/* Bloques */
	myfilesystem->sb->offset_bloques = myfilesystem->sb->offset_inodos + myfilesystem->sb->inode_reserved_blocks;
	printf("offset_bloques: %lu\n", myfilesystem->sb->offset_bloques);

	myfilesystem->sb->reserved_blocks = 1 + myfilesystem->sb->bitmapb_reserved_blocks + myfilesystem->sb->bitmapi_reserved_blocks + myfilesystem->sb->inode_reserved_blocks;
	printf("reserved_blocks: %lu\n", myfilesystem->sb->reserved_blocks);

	/*************************
	 * Inicializamos bitmaps *
	 *************************/

	/* Mapeamos espacio para el block_bitmap en el fichero */
	myfilesystem->block_bitmap.bitmap = (uint8_t *)mmap(NULL, myfilesystem->sb->bitmapb_reserved_blocks * BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, myfilesystem->sb->bitmapb_offset * BLOCK_SIZE);
	myfilesystem->block_bitmap.size = myfilesystem->sb->bitmapb_reserved_blocks * BLOCK_SIZE;   // Nº bloques totales / nº bits por byte

	/* Mapeamos espacio para el inode_bitmap en el fichero */
	myfilesystem->inode_bitmap.bitmap = (uint8_t *)mmap(NULL, myfilesystem->sb->bitmapi_reserved_blocks * BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, myfilesystem->sb->bitmapi_offset * BLOCK_SIZE);
	myfilesystem->inode_bitmap.size = myfilesystem->sb->bitmapi_reserved_blocks * BLOCK_SIZE;   // Nº bloques totales / nº bits por byte

	/* Seteamos a 1 los bloques reservados (probamos con 30)*/
	printf("Vamos a setear a 1 los bloques reservados...\n");
	int i;
	for (i = 0; i < 30; i++) {
		free_block(myfilesystem);  // Cambiar tipo a función free_block
	}

	/* Cerramos el fichero para el filesystem */
	close(file);

	printf("Todo guardado :D\nVamos a probar a mostrar algunos bits del block_bitmap :D\n");

	for (i = 0; i < 20; i++) {
		printf("Byte %d: %#x\n", i, myfilesystem->block_bitmap.bitmap[i]);
	}

	printf("Voy a probar a asignar algunos inodos\n");
	free_inode(myfilesystem);
	free_inode(myfilesystem);
	free_inode(myfilesystem);
	free_inode(myfilesystem);
	free_inode(myfilesystem);

	for (i = 0; i < 10; i++) {
		printf("Byte %d: %#x\n", i, myfilesystem->inode_bitmap.bitmap[i]);
	}

	fflush(stdout);

}











