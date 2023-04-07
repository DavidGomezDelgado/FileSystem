#include <sys/mman.h>
#include "filesystem.h"
#include <stdio.h>
#include <stdint.h>

int main ()
{
    directory_entry *test = malloc(sizeof(directory_entry));
    struct inode archivo;
    int i = strcpy(test->name, "prueba_test.txt");  // Ponemos nombre
}
