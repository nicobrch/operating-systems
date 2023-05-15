#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int main() {
    //  -- Rubrica --   //  -- Id 1 --  //
    string filename = "mochila.txt";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Couldn't open file" << endl;
        return 1;
    }

    int N = 0;
    string line;
    while (getline(file, line)) {
        ++N;    // contar filas para array
    }

    int column1_values[N];
    int column2_values[N];
    file.clear();
    file.seekg(0, ios::beg);    // empezar desde el principio

    int i = 0, ganancia = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        int val1, val2;
        ss >> val1 >> val2;
        column1_values[i] = val1;
        column2_values[i] = val2;
        if (val1>ganancia) ganancia = val1; // registro ganancia
        i++;
    }

    file.close();
    return 0;
}
