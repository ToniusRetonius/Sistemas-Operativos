#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

// en este ejercicio se quiere SIMULAR una comunicación mediante pipes entre dos procesos 
// valen las syscalls read() y write()
// tomar al pipe como si fuese un buffer de tamaño N con los métodos pop() y push()
// estos métodos se bloquean en caso de que no hayan mensajes o esté lleno

// #mensajes que tiene el buffer
int mensajes = 0;

// buffer en el que se puede por posición escribir
int simil_pipe[N];

// semáforo para tomar el simil_pipe y modificarlo también la #mensajes
sem_t pipe;

void proceso(int i){

    // bucle inf
    while (1){
        wait(pipe);
        // ver cuǽntos mensajes me mandó para leer
        if (mensajes > 0)
        {
            // asumimos que los mensajes se encolan en orden
            int lectura[mensajes];
            for (int i = 0; i < mensajes; i++)
            {
                int r = read(simil_pipe[i], &lectura[i], sizeof(simil_pipe[i]));
                simil_pipe[i].pop();
            }
            // le respondemos a nuestro proceso amigo
            int respuesta[] = generar_respuesta(lectura);

            for (int i = 0; i < (sizeof(respuesta) / sizeof(int)); i++)
            {
                int w = write(simil_pipe[i], respuesta[i], sizeof(int));
                simil_pipe[i].push();
            }
            // liberamos 
            signal(pipe);
        }
        else
        {
            // si no hay nada, le mando alguna boludez para que no haya deadlock
            int mensaje[] = saludar();

            for (int i = 0; i < (sizeof(mensaje) / sizeof(int)); i++)
            {
                int w = write(simil_pipe[i], mensaje[i], sizeof(int));
                simil_pipe[i].push();
            }
            // liberamos 
            signal(pipe);
        }
    }
}


int main(){
    // inicializar procesos
    for (int i = 0; i < 2; i++)
    {
        spawn proceso(i);
    }

    // libre el pipe
    pipe = 0;
    
}