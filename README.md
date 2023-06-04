# FileSystem
*** OBJETIVO ***
SABER GESTIONAR LOS BLOQUES EN TODO MOMENTO.

*** FUNCIONES IMPLEMENTADAS ***
- Obtener bit libre de inodo o bloque (bitmap.c)
- Crear inodo y crear inodo raíz (create_inode.c)
- Buscar fichero/directorio por inodo o por path (tree_manager.c)
- Cear directorio por path (directory_manager.c)
- Limpiar inodo, actualizar entrada de directorio padre y crear fichero por path (file_manager.c)
- Renombrar/mover fichero/directorio por path (file_manager.c) -> EN PROGRESO
- Eliminar inodo/entrada y eliminar fichero (file_remove.c)
- Escribir (sobreescribir) y leer en fichero (file_operations.c)
- Listar contenido de directorio (directory_operations.c)
- Avisar de error en parámetros para FUSE (error.c)

*** FUNCIONES A IMPLEMENTAR ***
- Modificar tamaño de fichero -> truncate.
- Mover fichero/directorio.
- Eliminar directorio recursivamente (OPCIONAL).
- Crear hardlinks y softlinks (OPCIONAL).

*** TODO ***
- Gestionar ficheros y directorios a partir de un path distinto al actual (SI HAY TIEMPO).
- Deberíamos hacer "mini funciones" para compactar el código (SI HAY TIEMPO).

*** ANOTACIONES ***
- Tener cuidado a la hora de manejar bitmap_bloques->bitmap. Manejamos bits, no bytes.
- En nuestro filesystem perdemos como mucho 7 bloques.

*** DUDAS ***
- Función modificar tamaño de fichero.
- ¿Cuánto espacio mapeamos para almacenar todos los bloques?
- ¿Seteamos a 1 el índice de un bloque cuando ya no tiene entradas libres?
