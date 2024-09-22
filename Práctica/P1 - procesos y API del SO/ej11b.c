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
int pipe_3[2];

int num = 0;

void hijo(){
    while (num < 5)
    {    
    // el hijo 1 lee del pipe 1 (donde escribe el padre)
    read(pipe_1[READ], &num, sizeof(num));
    // suma 1
    num++;
    printf("Hijo 1: %d\n", num);
    // escribe en el pipe 2 (donde lee el hijo 2)
    write(pipe_2[WRITE], &num, sizeof(num));
    }

    exit(0);
}
void hijo2(){
    // como nunca escribe en el pipe 2
    close(pipe_2[WRITE]);

    while (num < 5)
    {    
    // el hijo 2 lee del pipe 2 (donde escribe el hijo 1)
    read(pipe_2[READ], &num, sizeof(num));
    // suma 1
    num++;
    printf("Hijo 2: %d\n", num);
    // escribe en el pipe 2 (donde escribe él)
    write(pipe_3[WRITE], &num, sizeof(num));
    }

    exit(0);
}


int main(){
    // creamos los pipes
    pipe(pipe_1);
    pipe(pipe_2);
    pipe(pipe_3);

    // fork h1
    pid_t pid_1 = fork();
    if (pid_1 == -1) exit(1);
    else if (pid_1 == 0)
    {
        hijo();
    }

    // fork h2
    pid_t pid = fork();
    if (pid == -1) exit(1);
    else if (pid == 0)
    {
        hijo2();
    }

    // padre 
    while (num < 5)
    {    
    // le manda el número por el pipe 1 al hijo 1
    write(pipe_1[WRITE], &num, sizeof(num));

    // lee del extremo de lectura del pipe 3 (donde escribe el hijo 2)
    read(pipe_3[READ], &num, sizeof(num));
    
    // lectura h2
    printf("Recibí de mi hijo 2: %d\n", num);

    }

    exit(0);
    
}