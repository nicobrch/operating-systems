#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <limits.h>

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

bool gestionarPermisos(const string& path, mode_t permisos) {
    int gestionar = chmod(path.c_str(), permisos);
    if (gestionar == 0) {
        return true;
    } else {
        cout << "Error: No se pudieron cambiar los permisos." << endl;
        return false;
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

int main(){

    return 0;
}
