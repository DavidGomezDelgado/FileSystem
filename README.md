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
- Tener cuidado a la hora de manejar bitmap_bloques->bitmap. Manejamos bits, no bytes.

*** DUDAS ***
- Función modificar tamaño de fichero.
- ¿Hacemos malloc para myfilesystem?
