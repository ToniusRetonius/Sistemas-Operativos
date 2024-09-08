#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

// la tribu de antropófagos comen de una olla si hay comida, sino el cocinero debe poner comida
int olla = M;
sem_t sem_olla;
sem_t sem_cocinero;

void antropofago(){
    while (1)
    {
        // toma la porción de la olla, y come
        wait(sem_olla);
        if (olla > 0)
        {
            olla--;
            comer();
            signal(sem_olla);
        }
        else
        {
            // aviso al chef
            signal(sem_cocinero);
        }
    }
}

void cocinero(){
    while (1)
    {
        wait(sem_cocinero);
        // relleno la olla
        wait(sem_olla);
        if (olla == 0)
        {
            olla = M;
        }
        signal(sem_olla);
    }
    
}