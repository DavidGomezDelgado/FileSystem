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

*** FUNCIONES A IMPLEMENTAR ***
- Borrar directorio.  --> EN PROCESO DE PRUEBA
- Modificar tamaño de fichero.
- Cambiar nombre de fichero (también sirve para mover el fichero). -> DE MOMENTO SÓLO CAMBIAR NOMBRE
- Leer de fichero.  --> EN PROCESO
- Crear hardlinks y softlinks (OPCIONAL).

*** TODO Y ANOTACIONES ***
- Función leer de fichero.
- Tener cuidado a la hora de manejar bitmap_bloques->bitmap. Manejamos bits, no bytes.
- Continuar con _inode_search().
- Gestionar ficheros y directorios a partir de un path distinto al actual.

*** DUDAS ***
- Bloques reales negativos.
