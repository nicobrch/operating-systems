# Tarea 2
## Sistemas Operativos

Nicolás Chirino - Sección 1

### Ejecución

Para ejecutar el archivo, se puede hacer desde la shell con el comando:

```shell
g++ -o main main.cpp
./main
```

De forma alternativa, se puede ejecutar utilizando Docker, de la siguiente forma:

```shell
sudo docker build -t nombre_imagen .
sudo docker run -it nombre_imagen
```

De esta forma, se ejecutará el archivo en una terminal nueva utilizando docker y la última versión de GCC.
