# FileSystem
*** OBJETIVO: SABER GESTIONAR LOS BLOQUES EN TODO MOMENTO.

*** FUNCIONES IMPLEMENTADAS ***
- Crear inodo y crear inodo raíz (create_inode.c)
- Crear superbloque (create_inode.c)
- Buscar inodo y bloque libre (bitmap.c)
- Buscar inodo por nombre en directorio actual (tree_manager.c)
- Crear fichero y directorio en directorio actual (file_manager.c)
- Eliminar fichero (file_remove.c)
- Eliminar inodo y dentry (file_remove.c)
- Escribir (sobreescribir) en fichero (file_manager.c)
- Renombrar fichero o directorio (file_operations.c)

*** FUNCIONES A IMPLEMENTAR ***
- Modificar tamaño de fichero -> truncate.
- Crear hardlinks y softlinks (OPCIONAL).

*** TODO ***
- Modificar manejo de los bitmaps -> no accedemos a través de memoria, sino a través del fichero
- ¿Continuar con _inode_search()?
- Gestionar ficheros y directorios a partir de un path distinto al actual (SI HAY TIEMPO).
- Deberíamos hacer "mini funciones" para compactar el código (SI HAY TIEMPO).

*** ANOTACIONES ***
- En nuestro filesystem perdemos como mucho 7 bloques.
- Tenemos un Makefile en /Funcionable para mkFilesystem.c y la imagen.
- Tenemos otro Makefile en /FUSE para FileSystemFUSE.c.
- Compilamos y ejecutamos el mkFilesystem desde /Funcionable y creamos la imagen desde /Funcionable (se crea en /FUSE).
- Compilamos y ejecutamos el FileSystemFUSE desde /FUSE.

*** DUDAS ***
- Función modificar tamaño de fichero.
- ¿Hacemos malloc para myfilesystem?
- ¿Cuánto espacio mapeamos para almacenar todos los bloques?
- ¿Un bloque de entradas tiene 128 entradas?
- ¿Distintos punteros pueden acceder al mismo bloque? -> Ya que un bloque de entradas suponemos tiene 128 entradas
- ¿Seteamos a 1 el índice de un bloque cuando ya no tiene entradas libres o c?
