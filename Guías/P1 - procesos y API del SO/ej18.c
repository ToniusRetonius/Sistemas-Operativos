#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


void ejecutarHijo(int i, int pipes[][2]){
    // ..
}


int main(int argc, char* argv[]) {
    // se le pasa el valor de n por parámetro, sino ...
    if (argc < 2) { 
        printf ("Debe ejecutar con la cantidad de hijos como parametro\n");
        return 0; 
    }
    // capturamos la cantidad de subprocesos que se lanzan
    int N = atoi(argv[1]);

    // declaramos los pipes
    int pipes[N*2][2];  
    
    // inicializamos los pipes
    for ( int i=0; i< N*2; i++) {
        pipe(pipes[i]); 
    }

    // creamos los n-subprocesos
    for (int i=0; i < N; i++) {
        int pid = fork (); 
        if (pid == 0) {
            ejecutarHijo(i,pipes);
            return 0; 
        } else {
            // padre le pasa el numero al iésimo hijo por el pipe correspondiente
            int numero = dameNumero(pid);
            write(pipes[i][1], &numero, sizeof(numero));  
        } 
    }

    int cantidadTerminados = 0; 
    char hijoTermino [N] = {0};

    // mientras haya hijos ejecutando .. polling 
    while (cantidadTerminados < N) {
        for (int i=0; i < N; i++) {
            
            if (hijoTermino[i]) {
                continue; 
            }

            char termino = 0;
            write(pipes[i][1], &termino, sizeof(termino)); 
            read(pipes[N+i][0], &termino, sizeof(termino)); 
            
            // si terminó, informamos el resultado
            if (termino) {  
                int numero;
                int resultado;
                read(pipes[N+i][0], &numero, sizeof(numero));  
                read(pipes[N+i][0], &resultado, sizeof(resultado)); 
                informarResultado(numero, resultado); 
                hijoTermino[i] = 1;
                cantidadTerminados++; 
            } 
        } 
    }

    return 0; 
}
