#!/bin/bash
# parametros $1, $2, ... $n
p1=$1
p2=$2
# le sacamos los espacios al array de palabras
IFS=' ' read -r -a palabras <<< "$p2"

# podríamos chequear si respetan el formato ... (ni idea cómo)
# asumimos que int p1 y *char p2

res=()
# comparamos cada palabra con el valor de p1
for palabra in "${palabras[@]}"
do
	palabra_limpia=$(echo "$palabra" | tr -d '!,')
	len=${#palabra_limpia}
if (($len > $p1))
then
	# con rev invertimos la palabra, la forma de hacerlo es con pipe (|)
	palabra_invertida=$(echo "$palabra_limpia" | rev)
	# con tr transformamos según la opción de un set a otro
	palabra_invertida_mayuscula=$(echo "$palabra_invertida" | tr '[:lower:]' '[:upper:]')
	res+=("$palabra_invertida_mayuscula")
else
	res+=("$palabra_limpia")
fi
done

echo "${res[@]}"
