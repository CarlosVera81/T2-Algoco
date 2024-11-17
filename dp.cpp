#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <sys/resource.h>

using namespace std;

vector<int> cost_insert(26);
vector<int> cost_delete(26);
vector<vector<int>> cost_replace(26, vector<int>(26));
vector<vector<int>> cost_transpose(26, vector<int>(26));

void cargarCostos(const string &filename, vector<int> &costos) {
    ifstream file(filename);
    for (int &cost : costos) {
        file >> cost;
    }
    file.close();
}

void cargarCostos(const string &filename, vector<vector<int>> &costos) {
    ifstream file(filename);
    for (auto &fila : costos) {
        for (int &cost : fila) {
            file >> cost;
        }
    }
    file.close();
}

int cost_insertar(char a) {
    int index= int(a)-97;
    return cost_insert[index];
}

int cost_deletear(char a) {
    int index= int(a)-97;
    return cost_delete[index];
}

int cost_remplazar(char a, char b) {
    int index1= int(a)-97;
    int index2= int(b)-97;
    return cost_replace[index1][index2];
}

int cost_trans(char a, char b) {
    int index1= int(a)-97;
    int index2= int(b)-97;
    return cost_transpose[index1][index2];
}


pair<int, vector<string>> distanciaEdicion(const string &s1, const string &s2) {
    int m = s1.size();
    int n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    vector<vector<vector<string>>> operaciones(m + 1, vector<vector<string>>(n + 1));

    for (int i = 1; i <= m; ++i) {
        int costo = cost_deletear(s1[i - 1]);
        dp[i][0] = dp[i - 1][0] + costo;
        operaciones[i][0] = operaciones[i - 1][0];
        operaciones[i][0].push_back("Eliminar " + string(1, s1[i - 1]) + " (Costo: " + to_string(costo) + ")");
    }
    for (int j = 1; j <= n; ++j) {
        int costo = cost_insertar(s2[j - 1]);
        dp[0][j] = dp[0][j - 1] + costo;
        operaciones[0][j] = operaciones[0][j - 1];
        operaciones[0][j].push_back("Insertar " + string(1, s2[j - 1]) + " (Costo: " + to_string(costo) + ")");
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int costoIns = cost_insertar(s2[j - 1]);
            int costoDel = cost_deletear(s1[i - 1]);
            int costoSub = cost_remplazar(s1[i - 1], s2[j - 1]);

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

            if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1]) {
                int costoTrans = cost_trans(s1[i - 2], s1[i - 1]);
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

void procesarDataset(const string &filename) {
    ifstream file(filename);
    ofstream outfile("output_dp.txt"); 

    string line;
    
    chrono::duration<double, milli> promedio;

    while (getline(file, line)) {
        
        istringstream ss(line);
        string s1, s2;
        int n;
        
        
        if (getline(ss, s1, ',') && getline(ss, s2, ',') && ss >> n) {
            

            auto start = chrono::high_resolution_clock::now();
            auto [distancia, operaciones] = distanciaEdicion(s1, s2);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> duration = end - start;
            promedio+=duration;

            
            cout << "Strings: " << s1 << " , " << s2 << endl;
            cout << "Distancia calculada: " << distancia << ", Distancia esperada: " << n << endl;
            cout << "Tiempo de ejecución: " << duration.count() << " ms" << endl;

          
            outfile << "Strings: " << s1 << " , " << s2 << endl;
            outfile << "Distancia calculada: " << distancia << ", Distancia esperada: " << n << endl;
            if(distancia==n){
                outfile << "Resultado correcto" << endl;
            } else {
                outfile<< "Resultado incorrecto" << endl;
            }
            outfile << "Tiempo de ejecución: " << duration.count() << " ms" << endl;
            outfile << "Operaciones óptimas:" << endl;
            for (const auto &op : operaciones) {
                if (op.find("0")== string::npos){
                    outfile << " - " << op << endl;
                }
            }
            outfile << "--------------------------" << endl;
        }

    }

    cout << "Tiempo de ejecución promedio: " << promedio.count()/20 << " ms" << endl;
    outfile << "Tiempo promedio de ejecución: " << promedio.count()/20 <<" ms" << endl;

    file.close();
    outfile.close();
}

int main() {
   
    cargarCostos("cost_insert.txt", cost_insert);
    cargarCostos("cost_delete.txt", cost_delete);
    cargarCostos("cost_replace.txt", cost_replace);
    cargarCostos("cost_transpose.txt", cost_transpose);
    procesarDataset("dataset.txt");

    return 0;
}
