#include <iostream>
#include <fstream>
#include <sys/stat.h>

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

bool crearDirectorio(const string& dirname) {
    int result = mkdir(dirname.c_str(), 0777);
    if (result == 0) {
        return true;
    } else {
        cout << "Error: No se pudo crear el directorio." << endl;
        return false;
    }
}

int main(){

    return 0;
}
