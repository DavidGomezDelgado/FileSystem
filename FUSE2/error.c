#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

/* Analiza los parámetros pasados al montar el sistema de ficheros */
void error_parametros()
{
    fprintf(stderr, "uso:  basicFUSE [opciones FUSE] fichero_inicial punto_de_montaje\n");
    exit(-1);
}
