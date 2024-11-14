import numpy as np
from random import choice
from weighted_levenshtein import osa
from random import shuffle

# Función para leer el archivo de costos (una fila de 26 valores)
def leer_costos_unidimensional(archivo):
    with open(archivo, 'r') as f:
        return np.array([float(x) for x in f.readline().strip().split()], dtype=np.float64)

# Función para leer el archivo de costos de sustitución o transposición (matriz 26x26)
def leer_costos_matriciales(archivo):
    with open(archivo, 'r') as f:
        return np.array([list(map(float, line.strip().split())) for line in f.readlines()], dtype=np.float64)

# Función para crear una matriz de 128 valores para inserción y eliminación
def crear_fila_completa(costos_26):
    fila_completa = np.ones(128, dtype=np.float64)
    for i in range(26):
        fila_completa[ord('a') + i] = costos_26[i]
    return fila_completa

# Función para crear una matriz de 128x128 a partir de la matriz de 26x26
def crear_matriz_completa(costos_26x26):
    matriz_completa = np.ones((128, 128), dtype=np.float64)
    for i in range(26):
        for j in range(26):
            matriz_completa[ord('a') + i, ord('a') + j] = costos_26x26[i, j]
    return matriz_completa

# Cargar costos
insert_costs = leer_costos_unidimensional('cost_insert.txt')  
delete_costs = leer_costos_unidimensional('cost_delete.txt')  
substitute_costs = leer_costos_matriciales('cost_replace.txt')  
transpose_costs = leer_costos_matriciales('cost_transpose.txt')  

# Crear matrices completas de 128 para inserción, eliminación, sustitución y transposición
insert_costs_128 = crear_fila_completa(insert_costs)
delete_costs_128 = crear_fila_completa(delete_costs)
substitute_costs_128 = crear_matriz_completa(substitute_costs)
transpose_costs_128 = crear_matriz_completa(transpose_costs)

# Función para obtener un conjunto de palabras en inglés
def obtener_palabras_ingles():
    try:
        import nltk
        nltk.download('words')
        from nltk.corpus import words
        return [word.lower() for word in words.words() if word.islower() and len(word)<=8 and all('a' <= char <= 'z' for char in word)]
    except ImportError:
        print("No se pudo importar `nltk`. Usando lista de palabras estática.")
        return ["apple", "banana", "cherry", "date", "grape", "kiwi", "lemon", "melon", "orange", "pear"]

# Función para generar un dataset con casos de cadenas vacías
def generar_dataset_cadenas_vacias(num_casos):
    dataset = []
    for _ in range(num_casos):
        palabra1 = ""
        palabra2 = choice(obtener_palabras_ingles())
        costo = int(osa(palabra1, palabra2, insert_costs=insert_costs_128, 
                        delete_costs=delete_costs_128, 
                        substitute_costs=substitute_costs_128, 
                        transpose_costs=transpose_costs_128))
        dataset.append(f"{palabra1},{palabra2},{costo}")
    return dataset

# Función para generar un dataset con casos de caracteres repetidos más variados
def generar_dataset_caracteres_repetidos(num_casos):
    dataset = []
    for _ in range(num_casos):
        # Crear segmentos repetidos de letras aleatorias
        letra1 = choice("abcdefghijklmnopqrstuvwxyz")
        letra2 = choice("abcdefghijklmnopqrstuvwxyz")
        letra3 = choice("abcdefghijklmnopqrstuvwxyz")
        
        # Crear segmentos intercalados en lugar de concatenar directamente
        segmentos = (
            [letra1] * 3 + [letra2] * 2 + [letra3] * 3
        )
        
        # Barajar los elementos de los segmentos para crear palabras con patrones intercalados
        palabra1_segmentos = segmentos.copy()
        palabra2_segmentos = segmentos.copy()
        shuffle(palabra1_segmentos)
        shuffle(palabra2_segmentos)
        
        palabra1 = "".join(palabra1_segmentos)
        palabra2 = "".join(palabra2_segmentos)
        
        # Calcular el costo de edición entre las dos palabras
        costo = int(osa(palabra1, palabra2, insert_costs=insert_costs_128, 
                        delete_costs=delete_costs_128, 
                        substitute_costs=substitute_costs_128, 
                        transpose_costs=transpose_costs_128))
        dataset.append(f"{palabra1},{palabra2},{costo}")
    return dataset

# Función para generar un dataset con casos donde se requieren transposiciones
def generar_dataset_transposicionesP(num_casos):
    dataset = []
    for _ in range(num_casos):
        palabra1 = choice(["abcd", "efgh", "ijkl", "mnop", "qrst","uvwx"])
        palabra2 = "".join([palabra1[i+1] + palabra1[i] if i % 2 == 0 else "" for i in range(0, len(palabra1)-1, 2)])
        costo = int(osa(palabra1, palabra2, insert_costs=insert_costs_128, 
                        delete_costs=delete_costs_128, 
                        substitute_costs=substitute_costs_128, 
                        transpose_costs=transpose_costs_128))
        dataset.append(f"{palabra1},{palabra2},{costo}")
    return dataset


def generar_dataset_transposicionesG(num_casos):
    dataset = []
    for _ in range(num_casos):
        palabra1 = choice(["abcdefgh", "ijklmnop", "qrstuvwx"])
        palabra2 = "".join([palabra1[i+1] + palabra1[i] if i % 2 == 0 else "" for i in range(0, len(palabra1)-1, 2)])
        costo = int(osa(palabra1, palabra2, insert_costs=insert_costs_128, 
                        delete_costs=delete_costs_128, 
                        substitute_costs=substitute_costs_128, 
                        transpose_costs=transpose_costs_128))
        dataset.append(f"{palabra1},{palabra2},{costo}")
    return dataset

# Función general para generar dataset
def generar_dataset(palabras, num_palabras=20, tipo="aleatorio"):
    if tipo == "cadenas_vacias":
        return generar_dataset_cadenas_vacias(num_palabras)
    elif tipo == "caracteres_repetidos":
        return generar_dataset_caracteres_repetidos(num_palabras)
    elif tipo == "transposicionesP":
        return generar_dataset_transposicionesP(num_palabras)
    elif tipo == "transposicionesG":
        return generar_dataset_transposicionesG(num_palabras)
    else:  # caso aleatorio
        dataset = []
        for _ in range(num_palabras):
            palabra1 = choice(palabras)
            palabra2 = choice(palabras)
            costo = int(osa(palabra1, palabra2, insert_costs=insert_costs_128, 
                            delete_costs=delete_costs_128, 
                            substitute_costs=substitute_costs_128, 
                            transpose_costs=transpose_costs_128))
            dataset.append(f"{palabra1},{palabra2},{costo}")
        return dataset

# Función para guardar el dataset en un archivo
def guardar_dataset(dataset, archivo="dataset.txt"):
    with open(archivo, 'w') as f:
        for linea in dataset:
            f.write(linea + '\n')

# Obtener un conjunto de palabras en inglés
palabras_ingles = obtener_palabras_ingles()

# Generar el dataset específico
tipo_dataset = "aleatorio"  # Cambia el tipo a "cadenas_vacias", "caracteres_repetidos" o "transposiciones"
dataset = generar_dataset(palabras_ingles, num_palabras=20, tipo=tipo_dataset)

# Guardar el dataset en un archivo
guardar_dataset(dataset)

print(f"El dataset de tipo '{tipo_dataset}' ha sido guardado en 'dataset.txt' con {len(dataset)} registros.")
