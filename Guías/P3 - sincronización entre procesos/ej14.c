#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

// cantidad de gente dentro
int cant_gente;
sem_t sem_gente;

// limite municipal de 20personas
int limite_municipal = 20;

// sillón donde esperan
int sillon = 0;
sem_t sem_sillon;

// en el sillón se sienta aquel que lleva más tiempo parado
int fifo_parados[];
sem_t sem_fifo_parado;
// el que más tiempo en el sillón sentado es atendido primero
int fifo_sillon[];
sem_t sem_fifo_sillon;

// barbero libre
bool barbero_libre[3];
sem_t sem_barbero;

// el pago es de a una persona
bool caja_registradora;
sem_t caja;

// buffer de pago
int pago;
sem_t sem_pago;

void cliente(int nro){

    // en donde está el cliente ?
    bool sillon = false;
    bool parado = false;

    // esperamos para leer cuánta gente hay
    wait(sem_gente);
    
    if(cant_gente > limite_municipal) {
        // libero
        signal(sem_gente);
        // me voy
        salir();
    }
    else{
        // libero la lectura de la gente
        signal(sem_gente);
    }

    // me puedo sentar en el sillón ...?
    wait(sem_sillon);
    // sillón lleno
    if(sillon == 4) {
        // libero sillón
        signal(sem_sillon);
        // quiero agregarme a la lista de parados
        wait(sem_fifo_parado);
        fifo_parados.push();
        // libero
        signal(sem_fifo_parado);
        parado = true;
    }
    // sillón con lugar
    else
    {
        // me siento
        sillon++;
        // me agrego a la lista del sillón
        wait(sem_fifo_sillon);
        fifo_sillon.push();
        
        // libero
        signal(fifo_sillon);
        signal(sem_sillon);

        sillon = true;
    }

    while (parado)
    {
        // espero que se libere el sillón
        parado = mepuedosentarenelsillon(nro);
    }

    while (sillon)
    {
        // espero a ser atendido
        sillon = mepuedocortar(nro);
    }

    // por qué le paso el nro de cliente? las funciones estas comparan el primer valor de la fifo correspondiente para pasar al siguiente estadoy devuelven un bool si pueden y modifican el estado de la cola correspondiente
    
    // si salen del bucle entonces se cortan con alguno

    wait(sem_barbero);
    int barbero;
    for (int i = 0; i < 3; i++)
    {
        if (barbero_libre[i])
        {
            barbero = i;
            // aviso que no estará libre
            barbero_libre[barbero] = false;
            break;
        }
    }
    signal(sem_barbero);

    // me voy del sillón
    wait(sillon);
    sillon--;
    signal(sillon);

    // me corto con el iésimo barbero
    cortarCabello(barbero);

    // aviso que libero al barbero
    wait(sem_barbero);
    barbero_libre[barbero] = true;
    signal(sem_barbero);
    
    // pago 
    // esta funcion escribe en un buffer 
    pagar(barbero);

    // me voy
    wait(sem_gente);
    cant_gente--;
    signal(sem_gente);

}


void barbero(int i){
    while (1)
    {
        // chequeamos si hay gente en el sillón
        wait(sem_sillon);
        if (sillon > 0)
        {
            // libero
            signal(sem_sillon);
            // pido para modificar fifo_sillon
            wait(fifo_sillon);
            // hay alguien en el sillón y atiendo al primero
            int cliente = fifo_sillon.pop();
            signal(fifo_sillon);

            wait(sem_barbero);
            barbero_libre[i] = false;
            signal(sem_barbero);

            cortarCabello(cliente);
            aceptarPago(cliente);
            // una vez cortado el pelo, me libero
            wait(sem_barbero);
            barbero_libre[i] = true;
            signal(sem_barbero);
        }
    }
}