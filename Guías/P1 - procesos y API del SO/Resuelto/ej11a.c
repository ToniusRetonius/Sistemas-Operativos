#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>


// declaramos macros, 
enum {READ, WRITE};

// pipes declaramos globales
int pipe_1[2];
int pipe_2[2];

int num = 0;

// recibe el pipe como parámetro
void hijo(){
    while (num < 10)
    {    
    // el hijo lee del pipe 1 (donde escribe el padre)
    int num;
    read(pipe_1[READ], &num, sizeof(num));
    // suma 1
    num++;
    printf("Envié a mi padre el : %d\n", num);
    // escribe en el pipe 2 (donde escribe él)
    write(pipe_2[WRITE], &num, sizeof(num));
    }

    exit(0);
}


int main(){
    // creamos los pipes
    pipe(pipe_1);
    pipe(pipe_2);

    // fork
    pid_t pid = fork();
    if (pid == -1) exit(1);
    else if (pid == 0)
    {
        hijo();
    }

    while (num < 10)
    {    
    // padre 
    // le manda el número por el pipe 1
    write(pipe_1[WRITE], &num, sizeof(num));
    
    // lee del extremo de lectura del pipe 2 (donde escribe el hijo)
    read(pipe_2[READ], &num, sizeof(num));

    printf("Recibí de mi hijo: %d\n", num);
    }

    exit(0);
    
}