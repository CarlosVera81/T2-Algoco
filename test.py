def damerau_levenshtein_fuerza_bruta(s1, s2):
    """
    Calcula la distancia de Damerau-Levenshtein entre dos cadenas s1 y s2 utilizando fuerza bruta.
    Permite aplicar múltiples transformaciones en un mismo substring.
    """

    def transformaciones_fuerza_bruta(s):
        """
        Genera todas las posibles transformaciones de una cadena s aplicando las operaciones de
        inserción, eliminación, sustitución y transposición en cada posición.
        """
        n = len(s)
        transformaciones = set()

        # Generar inserciones
        for i in range(n + 1):
            for c in "abcdefghijklmnopqrstuvwxyz":
                transformaciones.add(s[:i] + c + s[i:])

        # Generar eliminaciones
        for i in range(n):
            transformaciones.add(s[:i] + s[i+1:])

        # Generar sustituciones
        for i in range(n):
            for c in "abcdefghijklmnopqrstuvwxyz":
                transformaciones.add(s[:i] + c + s[i+1:])

        # Generar transposiciones
        for i in range(n - 1):
            transformaciones.add(s[:i] + s[i+1] + s[i] + s[i+2:])

        return list(transformaciones)

    # Si las cadenas son iguales, la distancia es 0
    if s1 == s2:
        return 0

    # Usamos un conjunto para almacenar todas las transformaciones generadas
    visitados = set([s1])
    transformaciones = [(s1, 0)]

    # Realizamos una búsqueda en anchura para encontrar la menor distancia
    while transformaciones:
        actual, dist = transformaciones.pop(0)

        # Generamos todas las transformaciones posibles de la cadena actual
        for trans in transformaciones_fuerza_bruta(actual):
            # Si llegamos a la cadena objetivo, devolvemos la distancia
            if trans == s2:
                return dist + 1

            # Si no hemos visitado esta transformación, la añadimos para continuar explorando
            if trans not in visitados:
                visitados.add(trans)
                transformaciones.append((trans, dist + 1))

    # Si no se encuentra la cadena objetivo, devolvemos una distancia infinita
    return float('inf')

# Ejemplo de uso
s1 = "sociodramatic"
s2 = "glaringness"
print(f"Distancia Damerau-Levenshtein entre '{s1}' y '{s2}': {damerau_levenshtein_fuerza_bruta(s1, s2)}")
