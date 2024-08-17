// Ej 12 - plataformaCargaUsuarios
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>	

typedef struct usuario_s {
    char* nombre;    
    int8_t edad;     
    int32_t cuit;    
} usuario_t;

void escribirArchivo(usuario_t* user){
    // en vez del modo write, modo append, esto anda bien
    FILE* archivo = fopen("ej13.txt", "a");
    fprintf(archivo, "%s %hhd %d \n", user->nombre, user->edad, user->cuit);
    fclose(archivo);
}

usuario_t* leerArchivo(){
    FILE* archivo = fopen("ej12.txt", "r");

    usuario_t* lectura = malloc(sizeof(usuario_t));
    lectura->nombre = malloc(25 * sizeof(char));

    // https://www.w3schools.com/c/c_files_read.php
    char linea[256];
    fgets(linea, sizeof(linea), archivo);
    
    // sscanf se usa cuando tienes una cadena de texto con datos y deseas extraer esos datos en variables separadas. Es útil para procesar entradas que tienen un formato específico.
    sscanf(linea, "%24s %hhd %d", lectura->nombre, &lectura->edad, &lectura->cuit);

    fclose(archivo);
    return lectura;
}

int main(int argc, char *argv[])
{
    // ingresan 1 argumento : puede ser verInformacionUsuario o agregarInformacionUsuario
    // según el caso vamos a definir entonces
    char* entrada = argv[1];
    char* verInfo = "verInformacionUsuario";
    char* agregarInfo = "agregarInformacionUsuario";


    if ((strcmp(entrada, verInfo)) == 0)
    {
        usuario_t* usuarios = leerArchivo();
        if (usuarios == NULL)
        {
            printf("¡Ningún usuario fue cargado a la plataforma todavía!\n");
        } else
        {
            FILE* archivo = fopen("ej13.txt", "r");
            usuario_t* lectura = malloc(sizeof(usuario_t));
            lectura->nombre = malloc(25 * sizeof(char));
            char linea[256];
            while(fgets(linea, sizeof(linea), archivo)){
                sscanf(linea, "%24s %hhd %d", lectura->nombre, &lectura->edad, &lectura->cuit);
                printf("Nombre: %s, Edad: %hhd, Cuit: %d\n", lectura->nombre, lectura->edad, lectura->cuit);
            }
            fclose(archivo);    
        }

    } else if ((strcmp(entrada, agregarInfo)) == 0)
    {   
        // pedimos memoria
        usuario_t* user = malloc(sizeof(usuario_t));
        user->nombre = malloc(25 * sizeof(char));

        // Ingrese nombre de persona:
        printf("Ingrese nombre de persona:");
        scanf("%24s", user->nombre);

        // Ingrese edad:
        int8_t edad;
        printf("Ingrese edad:");
        scanf("%hhd", &edad);
        if (edad > 18 && edad < 99)
        {
            user->edad = edad;
        }else
        {
            printf("La información cargada es invalida, por favor, intente de nuevo\n");
            return 0;
        }
        
        int32_t cuit;
        // Ingrese cuit:
        printf("Ingrese cuit:");
        scanf("%d", &cuit);
        user->cuit = cuit;
        
        // tenemos que chequear el ej anterior para que funcione y luego implementarlo acǽ
        escribirArchivo(user);
        // ¡Su información fue cargada con éxito!
        printf("¡Su información fue cargada con éxito!\n");
    }
    else
    {
        printf("Poné una opción válida culeado! \n");
    }
    
    return 0;
}

