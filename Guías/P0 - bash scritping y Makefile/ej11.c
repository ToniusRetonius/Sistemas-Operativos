// ejercicio 11 - I/O standard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char  *argv[])
{
    // capturamos el cuit como un str
    char *cuit = argv[1];
    // los numeros por los cuales tenemos q multiplicar
    int numeros[] = {5,4,3,2,7,6,5,4,3,2};

    // nro verificador
    int verificador = 0;
    // son 10 numeros los que se multiplican 
    for (int i = 0; i < 10; i++)
    {
        int num = cuit[i] - '0';
        //printf("El num es %d\n", num);
        verificador+= num * numeros[i];
        //printf("El verificador es %d\n", verificador);
    }
    // division entera - 11
    verificador = (verificador / 11) - 11;
    //printf("El verificador es %d\n", verificador);

    // ultimo digito
    int udigit = cuit[10] - '0';
    //printf("El ultimo dig es %d\n", udigit);

    // chequeamos si es valido el cuit
    if (udigit == verificador)
    {
        printf("El cuit ingresado es VÁLIDO\n");
        
    } else
    {
        char cuit_corregido[14]; 
        snprintf(cuit_corregido, sizeof(cuit_corregido), "%2.2s-%8.8s-%1.1s", cuit, cuit + 2, cuit + 10);
        printf("El CUIT ingresado no es válido. Su información corregida es %s\n", cuit_corregido);
    }
    
    // hay algo raro con la forma de saber si es válido
    return 0;
}
