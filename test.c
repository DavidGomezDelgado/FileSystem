#include "filesystem.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int main()
{
    directory_entry *test_fran = (directory_entry *) malloc(sizeof(directory_entry)); // Le reservamos memoria
    struct inode *inodo = malloc(sizeof(struct inode));
    struct inode *inodo2 = malloc(sizeof(struct inode));
    //int directos[10],simples[1];
   // *inodo->i_directos = directos;
   // *inodo->i_simple_ind = simples;
    inodo->i_type = '-';
    inodo->i_tam = 0;

    strcpy(test_fran->name, "hola"); // Guardo el nombre
    test_fran->inode = inodo;

    long block_mem = (long) malloc(sizeof(directory_entry)); // Primer bloqueprueba.txt

    memcpy((void *)block_mem,test_fran,sizeof(directory_entry));
    printf("Bloque de memoria: %ld\n", block_mem);

    // Ya hemos mapeado nuestro bloque
    //directory_entry *traido_mem = (directory_entry *) mmap(NULL,sizeof(block_mem),PROT_WRITE,MAP_SHARED,*block_mem,0);

    directory_entry *traido_mem = malloc(sizeof(directory_entry));
    memcpy(traido_mem, (void *)block_mem,sizeof(directory_entry));

    printf("Entrada: %s\n", traido_mem->name);

    strcpy(test_fran->name, "hola2"); // Guardo el nombre
    test_fran->inode = inodo2;
    long block_mem2 = (long) malloc(sizeof(directory_entry)); // Primer bloqueprueba.txt

       memcpy((void *)block_mem2,test_fran,sizeof(directory_entry));
       printf("Bloque de memoria: %ld\n", block_mem2);

       // Ya hemos mapeado nuestro bloque
       //directory_entry *traido_mem = (directory_entry *) mmap(NULL,sizeof(block_mem),PROT_WRITE,MAP_SHARED,*block_mem,0);

       directory_entry *traido_mem2 = malloc(sizeof(directory_entry));
       memcpy(traido_mem2, (void *)block_mem2,sizeof(directory_entry));
       printf("Entrad2: %s\n", traido_mem2->name);
}
