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
- Control de bloques e inodos libres.
- Añadir atributo i_blocks (nº de bloques que direcciona) a estructura inode_fs.

- Corregir control de errores en algunas fucniones (devolver -1, 1... según error).
- Corregir funcionamiento de rename (al renombrar fichero a un nombre que ya tiene un directorio, se elimina el fichero), touch (cuando comprobamos si existe ya alguien con ese nombre, comprobar tb si es del mismo tipo).
- Eliminar condición if(path == "/") de rm porque el raíz nunca se puede eliminar.
- Corregir asignación de links en FUSE.

- Deberíamos hacer "mini funciones" para compactar el código (SI HAY TIEMPO).

*** ANOTACIONES ***
- En nuestro filesystem perdemos como mucho 7 bloques.
- Tenemos un Makefile en /Funcionable2.0 para mkFilesystem.c y la imagen.
- Tenemos otro Makefile en /FUSE2 para FileSystemFUSE.c.
- Compilamos y ejecutamos el mkFilesystem desde /Funcionable2.0 y creamos la imagen desde /Funcionable (se crea en /FUSE2).
- Compilamos y ejecutamos el FileSystemFUSE desde /FUSE2.

*** DUDAS ***
- Función modificar tamaño de fichero.
- ¿Seteamos a 1 el índice de un bloque cuando ya no tiene entradas libres o cómo?
