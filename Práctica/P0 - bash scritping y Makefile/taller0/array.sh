#!/bin/bash

# Inicializar arrays
declare -a nombres
declare -a edades
ciudades=()

# Leer el archivo CSV, omitir la primera línea (encabezado)
# while IFS=, read -r nombre edad ciudad; do ... done < datos.csv
# Cuando se usa read para leer una línea de texto, IFS determina 
# qué caracteres se usan para dividir esa línea en campos. por eso IFS=,

while IFS=, read nombre edad ciudad
 do
    # no nos interesa la primera 
    if [ "$nombre" != "nombre" ]
	then
	#agregamos
        nombres+=("$nombre")
        edades+=("$edad")
        ciudades+=("$ciudad")
    fi
done < datos.csv # leemos de cuál archivo ?

# Mostrar los arrays con todos sus elementos echo ${mi_array[@]}
echo "Nombres: ${nombres[@]}"
echo "Edades: ${edades[@]}"
echo "Ciudades: ${ciudades[@]}"



