# arkanoPi
Proyecto de la asignatura SDG2 del GITST UPM - Curso 2020/21

Proyecto que trata de construir un prototipo del juego de la pala inferior desplazable en la que rebota una pelota que trata de destruir ladrillos en la parte superior de la pantalla.
Idealmente se ejecutará en una Raspberry, pero este proyecto soporta su emulación en un PC sin necesidad de instalar hardware externo.

Para su desarrollo se está empleando Eclipse.

## Compilación del proyecto
Para compilar el proyecto se debe emplear un sistema base de tipo Unix y enlazar las librerias rt y pthread durante la compilación.

Ejemplo de compilación y ejecución sin IDEs:
```
rm ./arkanoPi
gcc *.c -o arkanoPi -lrt -lpthread
chmod +x ./arkanoPi
./arkanoPi
```
