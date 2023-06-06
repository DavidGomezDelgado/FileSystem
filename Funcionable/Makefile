#Variables
CC = gcc
CFLAGS=-Wall -I.

#NOmbre del fihcero objeto
TARGET=mkFileSystem

#Ficheros fuente
SOURCES= bitmap.c create_inode.c directory_manager.c directory_operations.c error.c file_manager.c file_operations.c file_remove.c mkFileSystem.c tree_manager.c

#Fichero objeto
OBJECT = $(TARGET).o

#Valor predeterminado de COUNT en caso de no pasarle ninguno por parámetro
DEFAULT_COUNT = 100

#Regla de compilación
#Para crear el objeto, dependemos de los sources.
#$^ hace referencia a lo que depende el objeto, es como poner $(SOURCE)
#$@ se refiere al objetivo actual, en este caso podríamos poner también $(OBJECT)

$(OBJECT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
	
image:
	dd if=/dev/zero of=../FUSE/filesystem.img bs=4096 count=$(COUNT)

clean:
	rm -f *.o $(OBJECT) $(TARGET) ../FUSE/filesystem.img
	
.PHONY: image clean

# Establecer valor predeterminado para COUNT en caso de que lo introduzcamos
ifndef COUNT
COUNT := $(DEFAULT_COUNT)
endif

