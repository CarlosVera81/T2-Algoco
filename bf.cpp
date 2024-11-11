#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <sstream>
#include <chrono>
#include <sys/resource.h> // Para medir el uso de memoria en UNIX

using namespace std;

// Variables globales para los costos
vector<int> cost_insert(26);
vector<int> cost_delete(26);
vector<vector<int>> cost_replace(26, vector<int>(26));
vector<vector<int>> cost_transpose(26, vector<int>(26));

// Funciones para cargar los costos desde los archivos
void cargarCostos(const string &filename, vector<int> &costos) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo " << filename << endl;
        exit(1);
    }
    for (int &cost : costos) {
        file >> cost;
    }
    file.close();
}

void cargarCostos(const string &filename, vector<vector<int>> &costos) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo " << filename << endl;
        exit(1);
    }
    for (auto &fila : costos) {
        for (int &cost : fila) {
            file >> cost;
        }
    }
    file.close();
}

// Funciones de costo
int costo_insercion(char b) {
    return cost_insert[b - 'a'];
}

int costo_eliminacion(char a) {
    return cost_delete[a - 'a'];
}

int costo_sustitucion(char a, char b) {
    return cost_replace[a - 'a'][b - 'a'];
}

int costo_transposicion(char a, char b) {
    return cost_transpose[a - 'a'][b - 'a'];
}

// Algoritmo de fuerza bruta para la distancia de edición extendida con registro de operaciones
int distanciaEdicionFuerzaBruta(const string &s1, const string &s2, int i, int j, vector<string> &operaciones) {
    if (i == int(s1.size()) && j == int(s2.size())) {
        return 0;
    }
    if (i == int(s1.size())) {
        int costo = costo_insercion(s2[j]);
        operaciones.push_back("Insertar '" + string(1, s2[j]) + "' (costo: " + to_string(costo) + ")");
        return costo + distanciaEdicionFuerzaBruta(s1, s2, i, j + 1, operaciones);
    }
    if (j == int(s2.size())) {
        int costo = costo_eliminacion(s1[i]);
        operaciones.push_back("Eliminar '" + string(1, s1[i]) + "' (costo: " + to_string(costo) + ")");
        return costo + distanciaEdicionFuerzaBruta(s1, s2, i + 1, j, operaciones);
    }

    vector<string> operacionesIns, operacionesDel, operacionesSub, operacionesTrans;
    
    int costoIns = costo_insercion(s2[j]) + distanciaEdicionFuerzaBruta(s1, s2, i, j + 1, operacionesIns);
    int costoDel = costo_eliminacion(s1[i]) + distanciaEdicionFuerzaBruta(s1, s2, i + 1, j, operacionesDel);
    int costoSub = costo_sustitucion(s1[i], s2[j]) + distanciaEdicionFuerzaBruta(s1, s2, i + 1, j + 1, operacionesSub);
    int costoTrans = INT_MAX;

    if (i + 1 < int(s1.size()) && j + 1 < int(s2.size()) && s1[i] == s2[j + 1] && s1[i + 1] == s2[j]) {
        costoTrans = costo_transposicion(s1[i], s1[i + 1]) + distanciaEdicionFuerzaBruta(s1, s2, i + 2, j + 2, operacionesTrans);
    }

    int costoMinimo = min({costoIns, costoDel, costoSub, costoTrans});

    if (costoMinimo == costoIns) {
        operaciones = operacionesIns;
        operaciones.push_back("Insertar '" + string(1, s2[j]) + "' (costo: " + to_string(costo_insercion(s2[j])) + ")");
    } else if (costoMinimo == costoDel) {
        operaciones = operacionesDel;
        operaciones.push_back("Eliminar '" + string(1, s1[i]) + "' (costo: " + to_string(costo_eliminacion(s1[i])) + ")");
    } else if (costoMinimo == costoSub) {
        operaciones = operacionesSub;
        operaciones.push_back("Sustituir '" + string(1, s1[i]) + "' por '" + string(1, s2[j]) + "' (costo: " + to_string(costo_sustitucion(s1[i], s2[j])) + ")");
    } else if (costoMinimo == costoTrans) {
        operaciones = operacionesTrans;
        operaciones.push_back("Transponer '" + string(1, s1[i]) + "' y '" + string(1, s1[i + 1]) + "' (costo: " + to_string(costo_transposicion(s1[i], s1[i + 1])) + ")");
    }

    return costoMinimo;
}

// Función para medir el uso de memoria


// Función para leer y procesar el dataset
void procesarDataset(const string &dataset_filename) {
    ifstream dataset_file(dataset_filename);
    ofstream output_file("output_bf.txt");  // Archivo de salida
    if (!dataset_file.is_open()) {
        cerr << "Error al abrir el archivo " << dataset_filename << endl;
        exit(1);
    }
    if (!output_file.is_open()) {
        cerr << "Error al abrir el archivo output_bf.txt" << endl;
        exit(1);
    }

    string line;
    while (getline(dataset_file, line)) {
        stringstream ss(line);
        string s1, s2;
        int expected_distance;

        getline(ss, s1, ',');
        getline(ss, s2, ',');
        ss >> expected_distance;

        vector<string> operaciones;

        // Medir tiempo y memoria
        auto start = chrono::high_resolution_clock::now();
        int calculated_distance = distanciaEdicionFuerzaBruta(s1, s2, 0, 0, operaciones);
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
     
        // Mostrar resultados en la consola
        cout << "Strings: \"" << s1 << "\" y \"" << s2 << "\"" << endl;
        cout << "Distancia esperada: " << expected_distance << ", Distancia calculada: " << calculated_distance << endl;
        cout << "Tiempo de ejecución: " << duration << " ms" << endl;
        
        // Escribir los resultados en el archivo de salida
        output_file << "Strings: \"" << s1 << "\" y \"" << s2 << "\"" << endl;
        output_file << "Distancia esperada: " << expected_distance << ", Distancia calculada: " << calculated_distance << endl;
        output_file << "Tiempo de ejecución: " << duration << " ms" << endl;
        output_file << "Secuencia de operaciones óptima:" << endl;
        
        for (const string &op : operaciones) {
            output_file << "  - " << op << endl;
        }
        
        output_file << "--------------------------" << endl;
    }
    dataset_file.close();
    output_file.close();
}

int main() {
    // Cargar los costos desde los archivos
    cargarCostos("cost_insert.txt", cost_insert);
    cargarCostos("cost_delete.txt", cost_delete);
    cargarCostos("cost_replace.txt", cost_replace);
    cargarCostos("cost_transpose.txt", cost_transpose);

    // Procesar el dataset y verificar los resultados
    procesarDataset("dataset.txt");

    return 0;
}
