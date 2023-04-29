# FileSystem
*** OBJETIVO: SABER GESTIONAR LOS BLOQUES EN TODO MOMENTO.

*** FUNCIONES QUE LLEVAMOS ***
- Crear inodo (create_inode.c)
- Crear root (create_inode.c)
- Crear superbloque (create_inode.c)
- Buscar inodo libre (bitmap.c)
- Buscar bloque libre (bitmap.c)
- Buscar inodo por nombre (tree_manager.c) --> CORREGIR

*** FUNCIONES A IMPLEMENTAR ***
- Crear y borrar fichero (con o sin path).
- Crear y borrar directorio (con o sin path).
- Modificar tamaño y contenido de fichero.
- Escribir en fichero.
- Leer de fichero.
- Crear hardlinks y softlinks (OPCIONAL).

*** ANOTACIONES ***
- 

*** DUDAS ***
- Fallo en memcpy.
- Uso de la estructura block.
- Función touch y edit_file.
