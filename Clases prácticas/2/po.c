#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

bool esPar(long numero) {
    return numero % 2 == 0;
}

long contarPares(long inicio, long fin) {
    long contador = 0;
    for (long i = inicio; i < fin; i++) {
        if (esPar(i)) contador++;
    }
    return contador;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero_de_procesos>\n", argv[0]);
        return 1;
    }

    int numProcesos = atoi(argv[1]);
    if (numProcesos <= 0) {
        fprintf(stderr, "El número de procesos debe ser mayor que 0.\n");
        return 1;
    }

    long inicioTotal = 2;
    long finTotal = 1000000000 + 1; 
    long rangoTotal = finTotal - inicioTotal;

    int pipeSubrango[numProcesos][2]; // padre -> hijo
    int pipeResultado[numProcesos][2]; // hijo -> padre

    pid_t pids[numProcesos];

    for (int i = 0; i < numProcesos; i++) {
        if (pipe(pipeSubrango[i]) == -1 || pipe(pipeResultado[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }

        if (pids[i] == 0) { 
            // hijo
            close(pipeSubrango[i][1]); 
            close(pipeResultado[i][0]); 

            long inicio, fin;
            read(pipeSubrango[i][0], &inicio, sizeof(long));
            read(pipeSubrango[i][0], &fin, sizeof(long));
            close(pipeSubrango[i][0]); 

            long resultado = contarPares(inicio, fin);
            // printf("Hijo PID %d: conté %ld números pares en el rango [%ld, %ld)\n", getpid(), resultado, inicio, fin);
            write(pipeResultado[i][1], &resultado, sizeof(long));
            close(pipeResultado[i][1]);
            exit(0);
        }

        close(pipeSubrango[i][0]);
        close(pipeResultado[i][1]);
    }

    long subrango = rangoTotal / numProcesos;
    long restante = rangoTotal % numProcesos;
    long inicio = inicioTotal;
    for (int i = 0; i < numProcesos; i++) {
        long fin = inicio + subrango;
        if (i < restante) fin++; 
        if (i == numProcesos - 1) fin = finTotal;

        write(pipeSubrango[i][1], &inicio, sizeof(long));
        write(pipeSubrango[i][1], &fin, sizeof(long));
        close(pipeSubrango[i][1]);
        
        inicio = fin; 
    }

    
    long total = 0;
    for (int i = 0; i < numProcesos; i++) {
        long parcial;
        read(pipeResultado[i][0], &parcial, sizeof(long));
        close(pipeResultado[i][0]);
        total += parcial;
    }

    printf("Cantidad total de números pares: %ld\n", total);

    return 0;
}
