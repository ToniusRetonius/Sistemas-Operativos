#include <stdlib.h>
#include <unistd.h>

// En el ejercicio 8 nos piden según el caso, escrbir el código que permita la ejecución de los procesos de la forma planteada usando SEMÁFOROS

#include <semaphore.h>

// 1 ) Procesos A, B y C :
// orden :  A - B - C - A - B - C ....
// 3 semáforos
sem_t semaforos[3];

// defino las macros según el proceso
#define A 0
#define B 1
#define C 2

void inicializar(){
    for (int i = 0; i < 3; i++)
    {
        semaforos[i] = 0;
    }
    // tendríamos que hacer un "spawn" de cada proceso acá
    // le damos comienzo al Proceso A
    semaforos[A].signal();
}
void a(){
    // espera - ejecuta - avisa al siguiente
    semaforos[A].wait();
    procesar_algo();
    semaforos[B].signal();
}
void b(){
    // espera - ejecuta - avisa al siguiente
    semaforos[B].wait();
    procesar_algo();
    semaforos[C].signal();
}
void c(){
    // espera - ejecuta - avisa al siguiente
    semaforos[C].wait();
    procesar_algo();
    semaforos[A].signal();
}


// 2 ) Procesos A, B y C :
// orden :  B - B - C - A - B - B - C - A - ....
// 3 semáforos
sem_t semaforos[3];

// defino las macros según el proceso
#define A 0
#define B 1
#define C 2

void inicializar(){
    for (int i = 0; i < 3; i++)
    {
        semaforos[i] = 0;
    }
    // tendríamos que hacer un "spawn" de cada proceso acá
    // le damos comienzo al Proceso A
    semaforos[B].signal();
}
void a(){
    // espera - ejecuta - avisa al siguiente
    semaforos[A].wait();
    procesar_algo();
    semaforos[B].signal();
}
void b(){
    // espera - ejecuta - avisa al siguiente
    semaforos[B].wait();
    // procesamos dos veces :
    procesar_algo();
    procesar_algo();
    semaforos[C].signal();
}
void c(){
    // espera - ejecuta - avisa al siguiente
    semaforos[C].wait();
    procesar_algo();
    semaforos[A].signal();
}

// 3 ) problema productor consumidor donde el proceso A es productor y B y C consumidores. 
// detalle : La información provista por el productor debe ser retirada siempre 2 veces

sem_t mutex;    // = 1
sem_t lleno;   // = 0
sem_t vacio;   // = 0
int cantidad;

void productor_A(){
    while (1)
    {
        // espera a que esté vacío y pone 2 
        sem_wait(vacio);

        // espera a tener acceso exclusivo para modificar 'cantidad'
        sem_wait(mutex);
        // ingresa a la sección crítica
        agregar();
        agregar();
        cantidad = 2;
        
        // damos el okay para que los consumidores morfen la data
        signal(mutex);
        // mandamos dos signal (uno por cada consumidor)
        signal(lleno);
        signal(lleno);
    }
}

void consumidor_B(){
    // espera a que el productor le de el okay para desbloquearse
    sem_wait(lleno);

    // necesita la posesión de la variable 
    sem_wait(mutex);
    cantidad--;
    if (cantidad == 0)
    {
        // avisamos la productor_A para que meta data
        signal(vacio);
    }
    // soltamos la variable exl
    signal(mutex);
}

void consumidor_C(){
    // espera a que el productor le de el okay para desbloquearse
    sem_wait(lleno);

    // necesita la posesión de la variable 
    sem_wait(mutex);
    cantidad--;
    if (cantidad == 0)
    {
        // avisamos la productor_A para que meta data
        signal(vacio);
    }
    // soltamos la variable exl
    signal(mutex);
}

// 4 ) ahora es similar, pero C saca 2 veces
void consumidor_C(){
    // espera a que el productor le de el okay para desbloquearse
    sem_wait(lleno);

    // necesita la posesión de la variable 
    sem_wait(mutex);
    if (cantidad == 2)
    {
        cantidad -= 2;
        // avisamos la productor_A para que meta data
        signal(vacio);
    }
    
    // soltamos la variable exl
    signal(mutex);
}