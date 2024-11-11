#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <sys/resource.h>
#include "sys/types.h"
#include "sys/sysinfo.h"

using namespace std;

vector<int> cost_insert(26);
vector<int> cost_delete(26);
vector<vector<int>> cost_replace(26, vector<int>(26));
vector<vector<int>> cost_transpose(26, vector<int>(26));

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

// Algoritmo de programación dinámica para la distancia de edición extendida
pair<int, vector<string>> distanciaEdicion(const string &s1, const string &s2) {
    int m = s1.size();
    int n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    vector<vector<vector<string>>> operaciones(m + 1, vector<vector<string>>(n + 1));

    // Inicialización de la primera fila y columna
    for (int i = 1; i <= m; ++i) {
        int costo = costo_eliminacion(s1[i - 1]);
        dp[i][0] = dp[i - 1][0] + costo;
        operaciones[i][0] = operaciones[i - 1][0];
        operaciones[i][0].push_back("Eliminar " + string(1, s1[i - 1]) + " (Costo: " + to_string(costo) + ")");
    }
    for (int j = 1; j <= n; ++j) {
        int costo = costo_insercion(s2[j - 1]);
        dp[0][j] = dp[0][j - 1] + costo;
        operaciones[0][j] = operaciones[0][j - 1];
        operaciones[0][j].push_back("Insertar " + string(1, s2[j - 1]) + " (Costo: " + to_string(costo) + ")");
    }

    // Cálculo de la distancia de edición con costos extendidos
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int costoIns = costo_insercion(s2[j - 1]);
            int costoDel = costo_eliminacion(s1[i - 1]);
            int costoSub = costo_sustitucion(s1[i - 1], s2[j - 1]);

            dp[i][j] = dp[i - 1][j - 1] + costoSub;
            operaciones[i][j] = operaciones[i - 1][j - 1];
            operaciones[i][j].push_back("Sustituir " + string(1, s1[i - 1]) + " por " + string(1, s2[j - 1]) + " (Costo: " + to_string(costoSub) + ")");

            if (dp[i][j - 1] + costoIns < dp[i][j]) {
                dp[i][j] = dp[i][j - 1] + costoIns;
                operaciones[i][j] = operaciones[i][j - 1];
                operaciones[i][j].push_back("Insertar " + string(1, s2[j - 1]) + " (Costo: " + to_string(costoIns) + ")");
            }
            if (dp[i - 1][j] + costoDel < dp[i][j]) {
                dp[i][j] = dp[i - 1][j] + costoDel;
                operaciones[i][j] = operaciones[i - 1][j];
                operaciones[i][j].push_back("Eliminar " + string(1, s1[i - 1]) + " (Costo: " + to_string(costoDel) + ")");
            }

            // Verificar la transposición si los caracteres son adyacentes
            if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1]) {
                int costoTrans = costo_transposicion(s1[i - 2], s1[i - 1]);
                if (dp[i - 2][j - 2] + costoTrans < dp[i][j]) {
                    dp[i][j] = dp[i - 2][j - 2] + costoTrans;
                    operaciones[i][j] = operaciones[i - 2][j - 2];
                    operaciones[i][j].push_back("Transponer " + string(1, s1[i - 2]) + " y " + string(1, s1[i - 1]) + " (Costo: " + to_string(costoTrans) + ")");
                }
            }
        }
    }

    return {dp[m][n], operaciones[m][n]};
}

// Función para leer el archivo dataset y calcular y comparar distancias
void procesarDataset(const string &filename) {
    long long TotalMemory=0;
    ifstream file(filename);
    ofstream outfile("output_dp.txt"); // Abrir archivo de salida
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo " << filename << endl;
        exit(1);
    }
    if (!outfile.is_open()) {
        cerr << "Error al abrir el archivo de salida" << endl;
        exit(1);
    }

    string line;
    
    while (getline(file, line)) {
        
        istringstream ss(line);
        string s1, s2;
        int n;

        // Separar la línea por comas y leer las variables
        if (getline(ss, s1, ',') && getline(ss, s2, ',') && ss >> n) {
            // Medir el tiempo de ejecución y el uso de memoria
            auto start = chrono::high_resolution_clock::now();
           
      
            // Calcular la distancia de edición con el algoritmo actual
            auto [distancia, operaciones] = distanciaEdicion(s1, s2);

            
           
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> duration = end - start;


            // Mostrar los resultados en pantalla
            cout << "Strings: " << s1 << ", " << s2 << endl;
            cout << "Distancia calculada: " << distancia << ", Distancia esperada: " << n << endl;
            cout << "Tiempo de ejecución: " << duration.count() << " ms" << endl;

            // Escribir los resultados en el archivo
            outfile << "Strings: " << s1 << ", " << s2 << endl;
            outfile << "Distancia calculada: " << distancia << ", Distancia esperada: " << n << endl;
            outfile << "Tiempo de ejecución: " << duration.count() << " ms" << endl;
            outfile << "Operaciones óptimas:" << endl;
            for (const auto &operacion : operaciones) {
                outfile << " - " << operacion << endl;
            }
            outfile << "----------------------------------------" << endl;
        } else {
            cerr << "Error al leer la línea: " << line << endl;
        }
    }

    file.close();
    outfile.close();
}

int main() {
    // Cargar los costos desde los archivos
    cargarCostos("cost_insert.txt", cost_insert);
    cargarCostos("cost_delete.txt", cost_delete);
    cargarCostos("cost_replace.txt", cost_replace);
    cargarCostos("cost_transpose.txt", cost_transpose);

    // Procesar el archivo dataset y calcular las distancias
    procesarDataset("dataset.txt");

    return 0;
}
