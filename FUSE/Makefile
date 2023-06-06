montaje = punto_montaje
fichero = filesystem.img

fuse_flags = -D_FILE_OFFSET_BITS=64 -lfuse -pthread

#Valor predeterminado de COUNT en caso de no pasarle ninguno por parámetro
DEFAULT_COUNT = 100

#Ficheros objeto
SOURCES = bitmap.o create_inode.o directory_manager.o directory_operations.o error.o file_manager.o file_operations.o file_remove.o tree_manager.o FilesystemFUSE.o

#Regla de compilación
#Para crear el objeto, dependemos de los sources.
#$^ hace referencia a lo que depende el objeto, es como poner $(SOURCE)
#$@ se refiere al objetivo actual, en este caso podríamos poner también $(OBJECT)
FilesystemFUSE: ${SOURCES}
	gcc -g -o $@ $^ ${fuse_flags}
	mkdir -p $(montaje)

#$< hace referencia a la primera dependencia, en este caso FilesystemFUSE.c
FilesystemFUSE.o: FilesystemFUSE.c filesystem.h
	gcc -g -c -o $@ $< ${fuse_flags}

bitmap.o: bitmap.c filesystem.h
	gcc -g -c -o $@  $<

create_inode.o: create_inode.c filesystem.h
	gcc -g -c -o $@  $<

directory_manager.o: directory_manager.c filesystem.h
	gcc -g -c -o $@  $<

directory_operations.o: directory_operations.c filesystem.h
	gcc -g -c -o $@  $<

error.o: error.c filesystem.h
	gcc -g -c -o $@  $<

file_manager.o: file_manager.c filesystem.h
	gcc -g -c -o $@  $<

file_operations.o: file_operations.c filesystem.h
	gcc -g -c -o $@  $<

file_remove.o: file_remove.c filesystem.h
	gcc -g -c -o $@  $<

tree_manager.o: tree_manager.c filesystem.h
	gcc -g -c -o $@  $<

image:
	dd if=/dev/zero of=filesystem.img bs=4096 count=$(COUNT)
	
mount: FilesystemFUSE
	./FilesystemFUSE $(fichero) $(montaje)
	
printf: FilesystemFUSE
	./FilesystemFUSE -f $(fichero) $(montaje)
	
clean:
	rm *.o
	
umount:
	fusermount -u $(montaje)

.PHONY: image clean

# Establecer valor predeterminado para COUNT en caso de que lo introduzcamos
ifndef COUNT
COUNT = $(DEFAULT_COUNT)
endif
