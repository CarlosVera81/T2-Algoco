#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <sstream>
#include <chrono>

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

int bf_osa(const string &s1, const string &s2, int i, int j, vector<string> &operaciones) {
    int m=s1.size();
    int n=s2.size();

    if (i == m && j == n) {
        return 0;
    }

    if (i == m) {
        operaciones.push_back("Insertar " + string(1, s2[j]) + " (Costo: " + to_string(cost_insertar(s2[j])) + ")");
        return cost_insertar(s2[j]) + bf_osa(s1, s2, i, j + 1, operaciones);
    }

    if (j == n) {
        operaciones.push_back("Eliminar " + string(1, s1[i]) + " (Costo: " + to_string(cost_deletear(s1[i])) + ")");
        return cost_deletear(s1[i]) + bf_osa(s1, s2, i + 1, j, operaciones);
    }

    vector<string> op_ins, op_del, op_sub, op_trans;
    int ins,del,sub,trans;

    if (i < m && j < n && s1[i] == s2[j]) {
        return bf_osa(s1, s2, i + 1, j + 1, operaciones);

    } else{

        ins = cost_insertar(s2[j]) + bf_osa(s1, s2, i, j + 1, op_ins);
        del = cost_deletear(s1[i]) + bf_osa(s1, s2, i + 1, j, op_del);
        sub = cost_remplazar(s1[i], s2[j]) + bf_osa(s1, s2, i + 1, j + 1, op_sub);
        trans = INT_MAX;

        if (i + 1 < m && j + 1 < n && s1[i] == s2[j + 1] && s1[i + 1] == s2[j]) {
            trans = cost_trans(s1[i], s1[i + 1]) + bf_osa(s1, s2, i + 2, j + 2, op_trans);
        }
    }

    int costo_min = min({ins, del, sub, trans});

    if (costo_min == ins) {
        operaciones = op_ins;
        operaciones.push_back("Insertar " + string(1, s2[j]) + " (Costo: " + to_string(cost_insertar(s2[j])) + ")");
    } else if (costo_min == del) {
        operaciones = op_del;
        operaciones.push_back("Eliminar " + string(1, s1[i]) + " (Costo: " + to_string(cost_deletear(s1[i])) + ")");
    } else if (costo_min == sub) {
        operaciones = op_sub;
        operaciones.push_back("Sustituir " + string(1, s1[i]) + " por " + string(1, s2[j]) + " (Costo: " + to_string(cost_remplazar(s1[i], s2[j])) + ")");
    } else if (costo_min == trans) {
        operaciones = op_trans;
        operaciones.push_back("Transponer " + string(1, s1[i]) + " y " + string(1, s1[i + 1]) + " (Costo: " + to_string(cost_trans(s1[i], s1[i + 1])) + ")");
    }

    return costo_min;
}


void procesarDataset(const string &dataset_filename) {
    ifstream dataset_file(dataset_filename);
    ofstream output_file("output_bf.txt");  
    
    string line;
    chrono::duration<double, milli> promedio;

    while (getline(dataset_file, line)) {
        stringstream ss(line);
        string s1, s2;
        int esperada;

        getline(ss, s1, ',');
        getline(ss, s2, ',');
        ss >> esperada;

        vector<string> operaciones;

   
        auto start = chrono::high_resolution_clock::now();
        int calculada = bf_osa(s1, s2, 0, 0, operaciones);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;
        promedio+=duration;

        cout << "Strings: " << s1 << " , " << s2 << endl;
        cout << "Distancia esperada: " << esperada << ", Distancia calculada: " << calculada << endl;
        cout << "Tiempo de ejecución: " << duration.count() << " ms" << endl;
        
  
        output_file << "Strings: " << s1 << " , " << s2 << endl;
        output_file << "Distancia calculada: " << calculada << ", Distancia esperada: " << esperada << endl;
        if(esperada==calculada){
            output_file << "Resultado correcto" << endl;
        } else {
            output_file << "Resultado incorrecto" << endl;
        }
        output_file << "Tiempo de ejecución: " << duration.count() << " ms" << endl;
        output_file << "Operaciones óptimas:" << endl;
        
        for (const string &op : operaciones) {
            if (op.find("0")== string::npos){
                output_file << " - " << op << endl;
            }
        }
        
        output_file << "--------------------------" << endl;
    }
    cout << "Tiempo de ejecución promedio: " << promedio.count()/20 << " ms" << endl;
    output_file << "Tiempo de ejecución promedio: " << promedio.count()/20 << " ms" << endl;

    dataset_file.close();
    output_file.close();
}

int main() {

    cargarCostos("cost_insert.txt", cost_insert);
    cargarCostos("cost_delete.txt", cost_delete);
    cargarCostos("cost_replace.txt", cost_replace);
    cargarCostos("cost_transpose.txt", cost_transpose);
    procesarDataset("dataset.txt");

    return 0;
}
