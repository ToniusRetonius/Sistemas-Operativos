#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define n 100

// el + 1 es para que siepre se avise a la siguiente y no tengamos que manejar el último caso con un if 
sem_t heladeras[n + 1];
sem_t barrera_llena;
sem_t botella = 0;
sem_t porron = 0;

// tengo que contar cuántas birras hay en la heladera
int birras = 0;
sem_t sem_birras = 1;

void init(){
    for(int i = 0; i < n; i++)
    {
        if (i == 0)
        {
            heladeras[i] = 1;
        }
        else
        {
            heladeras[i] = 0;
        }
    }
}

void birra(int i, char* tipo){
    // llega
    llegarABar();

    // me fijo el tipo
    if (strcmp(tipo, "botella") == 0)
    {
        wait(botella);
    }
    else
    {
        wait(porron);
    }

    // bancamos para leer de la cantidad total de birras agregadas en la i-ésima heladera
    wait(sem_birras);
    birras++;
    if (birras == 25)
    {
        // avisamos que ya estamos
        signal(barrera_llena);
    }
    signal(sem_birras);
}


void heladera(int i){
    // esperamos el turno
    wait(heladeras[i]);

    // enchufamos
    enchufar();

    // metemos 15 botellas de litro
    for (int i = 0; i < 15; i++)
    {
        // abrirla
        abrirHeladera();

        //meter la birra
        signal(botella);

        // cerrar la heladera
        cerrarHeladera();
    }
    // metemos 10 porrones
    for (int i = 0; i < 15; i++)
    {
        // abrirla
        abrirHeladera();

        //meter el porrón
        signal(porron);

        // cerrar la heladera
        cerrarHeladera();
    }

    // ya está llena ?
    wait(barrera_llena);

    // enfriamos
    enfriadoRapido();
    
    // ya podemos llenar la siguiente
    signal(heladeras[i+1]);
}
