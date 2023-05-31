#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

typedef struct{
	unsigned char *array;
	uint64_t size;
}bitmap_t;

int64_t setBit(bitmap_t *bitmap) {
    uint64_t i;
    unsigned char *p;

    for (i = 0, p = bitmap->array; i < bitmap->size; i++, p++) {
		printf("%#x\n",bitmap->array[i]);
        if (*p != 0xff) {
            break;
        }
    }
    printf("primer byte: %ld\n",i);

    if (i < bitmap->size) {
        for (int j = 0; j < 8; j++) {
            unsigned char mask = (0x01 << j);
            if (!(*p & mask)) {
                *p |= mask;
                return (i * 8) + j;  // Devolver la posición del bit encontrado
            }
        }
    } else {
        errno = ENOSPC;
        return -1;
    }

    // Si no se encuentra ningún bit disponible para establecer a 1
    errno = ENOSPC;
    return -1;
}

int main(){
	bitmap_t mib;
	char x[100];
	int i;
	mib.array = calloc(10, 1);
	mib.size = 10;
	for(i = 0; i < 10; i++)
		x[i] = setBit(&mib);
		
    for(i = 0; i < 10; i++)
		printf("%d\n", x[i]);
		
	printf("%#02x %#02x\n", mib.array[0], mib.array[1]);
	
	return 0;
}
