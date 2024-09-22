// Ej 14 - plataformaCargaUsuarios busca el cuit que le pasan
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>	

typedef struct usuario_s {
    char* nombre;    
    int8_t edad;     
    int32_t cuit;    
} usuario_t;

int main(int argc, char *argv[])
{
    // por argumento me pasan un CUIT
    int32_t cuit = atoi(argv[1]);

    // con este dato, tengo que encontrar un usuario que tenga este cuit
    FILE* archivo = fopen("ej13.txt", "r");
    // definimos al usuario
    usuario_t* user = malloc(sizeof(usuario_t));
    user->nombre = malloc(25*sizeof(char));
    
    // misma idea, traemos de a líneas
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo))
    {
        // las escaneamos, es decir agrupamos los datos que nos interesan
        sscanf(linea, "%24s %hhd %d", user->nombre, &user->edad, &user->cuit);
        if (user->cuit == cuit)
        {
            printf("Nombre: %s, Edad: %hhd, Cuit: %d\n", user->nombre, user->edad, user->cuit);
            return 0;
        }
    }
    printf("No se encontró ningún usuario con este cuit: %d\n", cuit);
    fclose(archivo);
    return 0;
}
