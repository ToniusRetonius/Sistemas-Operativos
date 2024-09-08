#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

// semáforo de entrada a la habitación
sem_t habitacion;

// los muchachos estǽn libres ?
bool libres = false;

// si la luz está prendida
bool luz = false;

// #prisioneros
int N;

// contador debe ser una variable compartida
sem_t count;
int contador;

void PrisioneroCero() {
    // es el primero ..  no es concurrente ?
    int contador = 0;

    // primera vez
    bool entreASala = false;

    while (!libres) {
        // Esperar que la sala esté libre
        wait(habitacion);
        if (!luz && !entreASala) {
            luz = true;
            wait(count);
            contador++;
            if (contador == N) libres = true;
            // liberamos
            signal(count);
            signal(habitacion);
        }
        // vale volver a entrar
        else
        {
            luz = true;
            // liberamos recurso
            signal(habitacion);
        } 
    }

    // si salimos del loop y le toca puede declarar que son libres
    printf("Somos libres !");
}

// Resto de los prisioneros
void RestoDeLosPrisionero(int i) {
    
    // esta variable captura si entró alguna vez
    bool entreASala = false;
    
    while (!libres) {
        // Esperar que la sala esté libre
        wait(habitacion);
        if (!entreASala) {
            // declaro que ya entré
            entreASala = true;
            // puedo hacer cualquier cosa con la luz
            luz = false;
            // espero la variable compartida
            wait(count);
            // la modifico
            contador++;
            // chequeamos si ya somos libres
            if (contador == N) libres = true;
            // la libero
            signal(count);

            // libero la habitación
            signal(habitacion);
        }
        // recordar que puedo entrar infinitamente
        else
        {
            luz = true;
            // libero la habitación
            signal(habitacion);
        }
    }

    // si somos libres
    printf("Somos libres xavales!");
}
