#!/bin/bash
echo -n 'Te gusta el mate? '
read mate
echo "Respondiste: $mate "
#hasta acá todo bien!
if [ "$mate" == "si" ]
then
	echo 'Aguante el mate'
else
	echo 'Alto bot'
fi

