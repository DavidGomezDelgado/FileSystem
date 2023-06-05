#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

void error_parametros()
{
    fprintf(stderr, "uso:  basicFUSE [opciones FUSE] fichero_inicial punto_de_montaje\n");
    exit(-1);
}
