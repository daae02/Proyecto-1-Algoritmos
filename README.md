# Proyecto 1 AlgoritmosCancel changes
 - Primer Proyecto de Analisis de Algoritmos Ingenieria en Computacion

# O grandes

 - Backtracking con dinámica: O(n^2)
 - Divide and Conquer: O(nlog2n)

# ¿Cuál es el más eficaz?

 Con 10 corridas:
 
 Valores:
 
 - Revisión en x y y: 0.25
 - Muestreo: 10px
 - Tolerancia: 10
 - % del probabilístico: 40%
 
 Análisis:
 
 - D&C: 5 coincidencias de 6 en un tiempo 0.082s - 0.085s
   > Porcentaje de accuracy: 83% en un tiempo promedio de 0.0835s
 
 - BT: 6 coincidencias de 6 en un tiempo constante de 0.08s
   > Porcentaje de accuracy: 100% en un tiempo constante de 0.08s
   
 - Probabilístico: 6-7 coincidencias de 6 en un tiempo de 0.011s-0.013s
   > Porcentaje de accuracy: 100% con un porcentaje de fallo del 0.08% en un tiempo promedio de 0.012s

Viendo este análisis se puede concluir que el algoritmo menos costoso y preciso es el de Backtracking con programación dinámica, ya que gracias a la implementación de   programación dinámica con cada iteración se optimiza el algoritmo descaratando elementos, al mismo tiempo que mantiene la precición del algoritmo de backtracking.
