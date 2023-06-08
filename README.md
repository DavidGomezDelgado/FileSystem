# FileSystem
*** OBJETIVO: SABER GESTIONAR LOS BLOQUES EN TODO MOMENTO.


*** OPERACIONES ***

- Crear y eliminar ficheros.
- Crear y eliminar directorios (vacíos).
- Escribir y leer de ficheros.
- Renombrar ficheros y directorios.
- Lista el contenido de un directorio.


*** ANOTACIONES ***

- En la carpeta Funcionable2.0 se encunetran un Makefile y todas las funciones utilizadas para formatear el sistema de ficheros PorygonEXT. Este formateador es mkFileSystem.c.
- En la carpeta FUSE2 se encuentran un Makefile y los mismos códigos necesarios para FUSE. El código implementado en FUSE es FilesystemFUSE.c.
- Compilamos y ejecutamos el mkFilesystem.c desde /Funcionable2.0 y creamos la imagen desde /Funcionable2.0 (se crea en /FUSE2).
- Compilamos y ejecutamos el FileSystemFUSE.c desde /FUSE2.


*** CÓMO USAR EL SISTEMA DE FICHEROS ***

En la carpeta Funcionable2.0:

1º Limpiar cualquier rastro de un montaje anterior:
 $ make clean
 
2º Crear la imagen de nuestro sistema de ficheros (se crea en la carpeta FUSE2):
 $ make image
 
3º Si se realiza algún cambio en el formateador, compilarlo:
 $ make 

4º Ejecutamos el formateador mkFileSystem.c:
 $ ./mkFileSystem PorygonEXT
 
En la carpeta FUSE2:

5º Limpiar cualquier rastro de un montaje anterior:
 $ make clean
 
6º Si se ha realizado algún cambio en el código de FUSE, compilarlo:
 $ make
 
7º Montar el sistema de ficheros:
 $ make mount  ->  Ejecutar en otra terminal " $ make debug " si se desea seguir una depuración del código
 
8º Entrar al punto de montaje creado y realizar las operaciones mencionadas en el apartado OPERACIONES. Una vez acabado, salir del punto de montaje.

9º Desmontar el sistema de ficheros:
 % make umount
 
10º Eliminar archivos creados durante la compilación:
 $ make clean

En la carpeta Funcionable2.0:

11º Eliminar la imagen del sistema y archivos creados durante la compilación:
 $ make clean
 

