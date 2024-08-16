#!/bin/bash
echo -n 'Dame un numero: '
read num1
a=num1
echo -n 'Dame otro numero: '
read num2
b=num2
suma=$(($a+$b))
producto=$(($a*$b))
echo "La suma de los número que me pasaste es = $suma"
echo "El producto de los números que me pasaste es = $producto"
if (($suma >= $producto))
then
	echo "La suma = $suma es mayor o igual que $producto"
else
	echo "La suma = $suma es menor que $producto"
fi
