#include <iostream>
#include <unordered_set>
#include <queue>
#include <string>
#include <vector>
#include <climits>

using namespace std;

vector<string> generarTransformaciones(const string& s) {
    vector<string> transformaciones;
    int n = s.size();

    // Inserciones
    for (int i = 0; i <= n; ++i) {
        for (char c = 'a'; c <= 'z'; ++c) {
            string nueva = s.substr(0, i) + c + s.substr(i);
            transformaciones.push_back(nueva);
        }
    }

    // Eliminaciones
    for (int i = 0; i < n; ++i) {
        string nueva = s.substr(0, i) + s.substr(i + 1);
        transformaciones.push_back(nueva);
    }

    // Sustituciones
    for (int i = 0; i < n; ++i) {
        for (char c = 'a'; c <= 'z'; ++c) {
            string nueva = s.substr(0, i) + c + s.substr(i + 1);
            transformaciones.push_back(nueva);
        }
    }

    // Transposiciones
    for (int i = 0; i < n - 1; ++i) {
        string nueva = s;
        swap(nueva[i], nueva[i + 1]);
        transformaciones.push_back(nueva);
    }

    return transformaciones;
}

int damerauLevenshteinFuerzaBruta(const string& s1, const string& s2) {
    // Si las cadenas son iguales, la distancia es 0
    if (s1 == s2) {
        return 0;
    }

    // Conjunto para almacenar las transformaciones visitadas y evitar duplicados
    unordered_set<string> visitados;
    visitados.insert(s1);

    // Cola para la búsqueda en anchura, almacenando cada transformación y su distancia
    queue<pair<string, int>> cola;
    cola.push({s1, 0});

    // Realizar la búsqueda en anchura
    while (!cola.empty()) {
        auto [actual, dist] = cola.front();
        cola.pop();

        // Generar todas las transformaciones posibles de la cadena actual
        vector<string> transformaciones = generarTransformaciones(actual);

        for (const string& trans : transformaciones) {
            // Si encontramos la cadena objetivo, devolvemos la distancia
            if (trans == s2) {
                return dist + 1;
            }

            // Si no hemos visitado esta transformación, la añadimos para continuar explorando
            if (visitados.find(trans) == visitados.end()) {
                visitados.insert(trans);
                cola.push({trans, dist + 1});
            }
        }
    }

    // Si no se encuentra la cadena objetivo, devolvemos una distancia infinita
    return INT_MAX;
}

int main() {
    string s1 = "sociodramatic";
    string s2 = "glaringness";

    int distancia = damerauLevenshteinFuerzaBruta(s1, s2);
    cout << "Distancia Damerau-Levenshtein entre '" << s1 << "' y '" << s2 << "': " << distancia << endl;

    return 0;
}
