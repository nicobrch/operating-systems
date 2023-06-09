#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <limits.h>
#include <vector>

using namespace std;

bool crearArchivo(const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (archivo) {
        archivo.close();
        return true;
    } else {
        cout << "Error: No se pudo crear el archivo." << endl;
        return false;
    }
}

bool crearDirectorio(const string& nombreDirectorio) {
    int directorio = mkdir(nombreDirectorio.c_str(), 0777);
    if (directorio == 0) {
        return true;
    } else {
        cout << "Error: No se pudo crear el directorio." << endl;
        return false;
    }
}

bool gestionarPermisos(const string& path, string permisos) {
    int gestionar = chmod(path.c_str(), strtol(permisos.c_str(), nullptr, 8));
    if (gestionar == 0) {
        return true;
    } else {
        cout << "Error: No se pudieron cambiar los permisos." << endl;
        return false;
    }
}

void listarArchivos(const string& path) {
    DIR* directorio = opendir(path.c_str());
    if (directorio) {
        struct dirent* archivo;
        while ((archivo = readdir(directorio)) != nullptr) {
            string nombreArchivo = archivo->d_name;
            cout << nombreArchivo << endl;
        }

        closedir(directorio);
    } else {
        cout << "Error: No se pudo abrir el directorio." << endl;
    }
}

void listarInodos(const string& path) {
    DIR* directorio = opendir(path.c_str());
    if (directorio) {
        struct dirent* archivo;
        while ((archivo = readdir(directorio)) != nullptr) {
            string nombreArchivo = archivo->d_name;
            string rutaArchivo = path + "/" + nombreArchivo;

            struct stat inodo;
            if (stat(rutaArchivo.c_str(), &inodo) == 0) {
                cout << archivo->d_ino << "\t" << nombreArchivo << endl;
            }
        }

        closedir(directorio);
    } else {
        cout << "Error: No se pudo abrir el directorio." << endl;
    }
}

void listarRecursivo(const string& path) {
    DIR* directorio = opendir(path.c_str());
    if (directorio) {
        struct dirent* archivo;
        while ((archivo = readdir(directorio)) != nullptr) {
            if (strcmp(archivo->d_name, ".") != 0 && strcmp(archivo->d_name, "..") != 0) {
                string nombreArchivo = archivo->d_name;
                string rutaArchivo = path + "/" + nombreArchivo;

                struct stat inodo;
                if (stat(rutaArchivo.c_str(), &inodo) == 0) {
                    if (S_ISDIR(inodo.st_mode)) {
                        cout << rutaArchivo << ":" << endl;
                        listarRecursivo(rutaArchivo);
                    } else {
                        cout << rutaArchivo << endl;
                    }
                }
            }
        }

        closedir(directorio);
    } else {
        cout << "Error: No se pudo abrir el directorio." << endl;
    }
}

string obtenerRutaActual() {
    char path[PATH_MAX];
    if (getcwd(path, sizeof(path)) != nullptr) {
        return string(path);
    } else {
        cerr << "Error: No se pudo obtener la ruta actual." << endl;
        return "";
    }
}

bool cambiarDirectorio(const string& path) {
    if (path == "~") {
        const char* directorioHome = getenv("HOME");
        if (directorioHome != nullptr) {
            return chdir(directorioHome) == 0;
        }
    } else if (path == "..") {
        string directorioActual = obtenerRutaActual();
        size_t slashPrevio = directorioActual.rfind('/');
        if (slashPrevio != string::npos) {
            string directorioPadre = directorioActual.substr(0, slashPrevio);
            return chdir(directorioPadre.c_str()) == 0;
        }
    } else {
        struct stat inodo;
        if (stat(path.c_str(), &inodo) == 0 && S_ISDIR(inodo.st_mode)) {
            return chdir(path.c_str()) == 0;
        }
    }

    return false;
}

bool borrarArchivo(const string& path) {
    return unlink(path.c_str()) == 0;
}

bool borrarDirectorio(const string& path) {
    DIR* directorio = opendir(path.c_str());
    if (directorio) {
        struct dirent* archivo;
        while ((archivo = readdir(directorio)) != nullptr) {
            if (strcmp(archivo->d_name, ".") != 0 && strcmp(archivo->d_name, "..") != 0) {
                string nombreArchivo = archivo->d_name;
                string rutaArchivo = path + "/" + nombreArchivo;

                struct stat inodo;
                if (stat(rutaArchivo.c_str(), &inodo) == 0) {
                    if (S_ISDIR(inodo.st_mode)) {
                        borrarDirectorio(rutaArchivo);
                    } else {
                        unlink(rutaArchivo.c_str());
                    }
                }
            }
        }

        closedir(directorio);

        if (rmdir(path.c_str()) == 0) {
            return true;
        }
    }

    return false;
}

bool mover(const string& sourcePath, const string& destinationPath) {
    return rename(sourcePath.c_str(), destinationPath.c_str()) == 0;
}

void help() {
    cout << "\033[1mComandos disponibles:\033[0m" << endl;
    cout << "\033[1m- touch <nombre_archivo>\033[0m: crea un nuevo archivo." << endl;
    cout << "\033[1m- mkdir <nombre_directorio>\033[0m: crea un nuevo directorio." << endl;
    cout << "\033[1m- chmod <ruta> <permisos>\033[0m: cambia los permisos de un archivo o directorio." << endl;
    cout << "\033[1m- ls\033[0m: lista los archivos y directorios en el directorio actual." << endl;
    cout << "\033[1m- ls -i\033[0m: lista los archivos y directorios con sus respectivos i-nodos en el directorio actual." << endl;
    cout << "\033[1m- ls -R\033[0m: lista de forma recursiva los archivos y directorios en el directorio actual y sus subdirectorios." << endl;
    cout << "\033[1m- cd <ruta>\033[0m: cambia el directorio actual." << endl;
    cout << "\033[1m- rm <ruta>\033[0m: elimina un archivo." << endl;
    cout << "\033[1m- rm-r <ruta>\033[0m: elimina un directorio y su contenido de forma recursiva." << endl;
    cout << "\033[1m- mv <ruta_origen> <ruta_destino>\033[0m: mueve un archivo o directorio a una nueva ubicación." << endl;
    cout << "\033[1m- print\033[0m: imprime el árbol de archivos y directorios." << endl;
    cout << "\033[1m- help\033[0m: lista de comandos y sus funciones." << endl;
    cout << "\033[1m- exit\033[0m: sale del programa." << endl;
}

struct FileNode {
    string name;
    string id;
    string permissions;
    size_t size;
    vector<FileNode> children;
};

class ArbolFS {
private:
    FileNode root;

public:
    ArbolFS() {
        // Obtener la ruta actual y construir el árbol de archivos desde allí
        string directorioActual = obtenerRutaActual();
        crearArbol(directorioActual, root);
    }

    void crearArbol(const string& rutaDirectorio, FileNode& nodoPadre) {
        DIR* directorio = opendir(rutaDirectorio.c_str());
        if (directorio) {
            struct dirent* archivo;
            while ((archivo = readdir(directorio)) != nullptr) {
                if (strcmp(archivo->d_name, ".") != 0 && strcmp(archivo->d_name, "..") != 0) {
                    string nombreArchivo = archivo->d_name;
                    string rutaArchivo = rutaDirectorio + "/" + nombreArchivo;

                    struct stat inodo;
                    if (stat(rutaArchivo.c_str(), &inodo) == 0) {
                        FileNode nodoArchivo;
                        nodoArchivo.name = nombreArchivo;
                        nodoArchivo.id = to_string(inodo.st_ino);
                        nodoArchivo.permissions = getPermissionsString(inodo.st_mode);
                        nodoArchivo.size = inodo.st_size;

                        if (S_ISDIR(inodo.st_mode)) {
                            crearArbol(rutaArchivo, nodoArchivo);
                        }

                        nodoPadre.children.push_back(nodoArchivo);
                    }
                }
            }

            closedir(directorio);
        }
    }

    string getPermissionsString(mode_t mode) {
        string permisos;

        permisos += (S_ISDIR(mode)) ? "d" : "-";
        permisos += (mode & S_IRUSR) ? "r" : "-";
        permisos += (mode & S_IWUSR) ? "w" : "-";
        permisos += (mode & S_IXUSR) ? "x" : "-";
        permisos += (mode & S_IRGRP) ? "r" : "-";
        permisos += (mode & S_IWGRP) ? "w" : "-";
        permisos += (mode & S_IXGRP) ? "x" : "-";
        permisos += (mode & S_IROTH) ? "r" : "-";
        permisos += (mode & S_IWOTH) ? "w" : "-";
        permisos += (mode & S_IXOTH) ? "x" : "-";

        return permisos;
    }

    void imprimirArbol() {
        imprimirNodo(root, 0);
    }

    void imprimirNodo(const FileNode& node, int level) {
        cout << string(level * 2, ' ') << "- " << node.name << " (ID: " << node.id << ", Size: " << node.size << " bytes, Permisos: " << node.permissions << ")" << endl;

        for (const FileNode& child : node.children) {
            imprimirNodo(child, level + 1);
        }
    }
};

int main() {
    ArbolFS arbol;
    string entrada;

    while (true) {
        string rutaActual = obtenerRutaActual();
        cout << rutaActual << " $ ";
        getline(cin, entrada);

        if (entrada == "exit") {
            cout << "Saliendo del programa." << endl;
            break;
        } else if (entrada == "help") {
            help();
        } else if (entrada == "print") {
            arbol.imprimirArbol();
        } else if (entrada.substr(0, 6) == "touch ") {
            string nombreArchivo = entrada.substr(6);
            cout << "Creando archivo: " << nombreArchivo << endl;
            crearArchivo(nombreArchivo);
        } else if (entrada.substr(0, 6) == "mkdir ") {
            string nombreDirectorio = entrada.substr(6);
            cout << "Creando directorio: " << nombreDirectorio << endl;
            crearDirectorio(nombreDirectorio);
        } else if (entrada.substr(0, 6) == "chmod ") {
            string path = entrada.substr(6);
            string permisos;
            cout << "Ingrese los permisos (por ejemplo: rwxr-xr-x): ";
            getline(cin, permisos);
            cout << "Cambiando permisos de: " << path << " a " << permisos << endl;
            gestionarPermisos(path, permisos);
        } else if (entrada == "ls") {
            cout << "Listando archivos y directorios:" << endl;
            listarArchivos(rutaActual);
        } else if (entrada == "ls -i") {
            cout << "Listando archivos y directorios con i-nodos:" << endl;
            listarInodos(rutaActual);
        } else if (entrada == "ls -R") {
            cout << "Listando archivos y directorios de forma recursiva:" << endl;
            listarRecursivo(rutaActual);
        } else if (entrada.substr(0, 3) == "cd ") {
            string path = entrada.substr(3);
            cout << "Cambiando directorio a: " << path << endl;
            cambiarDirectorio(path);
        } else if (entrada.substr(0, 3) == "rm ") {
            string path = entrada.substr(3);
            cout << "Eliminando archivo: " << path << endl;
            borrarArchivo(path);
        } else if (entrada.substr(0, 5) == "rm-r ") {
            string path = entrada.substr(5);
            cout << "Eliminando directorio recursivamente: " << path << endl;
            borrarDirectorio(path);
        } else if (entrada.substr(0, 3) == "mv ") {
            string rutaOrigen, rutaDestino;
            size_t posicion = entrada.find(' ', 3);
            rutaOrigen = entrada.substr(3, posicion - 3);
            rutaDestino = entrada.substr(posicion + 1);
            cout << "Moviendo: " << rutaOrigen << " a " << rutaDestino << endl;
            mover(rutaOrigen, rutaDestino);
        } else {
            cout << "Comando no reconocido. Por favor, inténtelo de nuevo." << endl;
        }
    }

    return 0;
}
