#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}

void hijo_preferido(int pipes_anillo[][2], int pipe_hijo[], int magico, int indice, int n){
	// primer iteración
	int lectura;
	int tomas = read(pipe_hijo[PIPE_READ], &lectura, sizeof(int));
	lectura++;
	int tomasulo = write(pipes_anillo[indice+1][PIPE_WRITE], &lectura, sizeof(int));

	// otras
	while(1)
	{
		int lectura_anterior;
		int sss = read(pipes_anillo[(indice-1) % n][PIPE_READ], &lectura_anterior, sizeof(int));

		if (lectura_anterior > magico)
		{
			int es = write(pipe_hijo[PIPE_WRITE], &lectura_anterior, sizeof(int));
			exit(0);
		}
		lectura_anterior++;
		int juanse = write(pipes_anillo[indice+1][PIPE_WRITE], &lectura_anterior, sizeof(int));
	}
}

void hijo(int pipes_anillo[][2], int indice, int n){
	while (1)
	{
		int lectura_anterior;
		int k = read(pipes_anillo[(indice-1) % n][PIPE_READ], &lectura_anterior, sizeof(int));
		lectura_anterior++;
		int t = write(pipes_anillo[indice+1][PIPE_WRITE], &lectura_anterior, sizeof(int));
	}	
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
	// declaramos el arreglo de pipes 
	int pipes_anillo[n][2];

	for (int j = 0; j < n; j++)
	{
		pipe(pipes_anillo[j]);
	}

	// pipe para comunicar padre con start
	int pipe_hijo[2];
	pipe(pipe_hijo);

	// forks
	for (int i = 0; i < n; i++)
	{
		pid_t pid = fork();
		if (pid == -1)exit(1);
		if (pid == 0)
		{
			if(i == start){
				int magico = generate_random_number();
				printf("%d", magico);
				hijo_preferido(pipes_anillo, pipe_hijo, magico, i, n);
			}else{
				hijo(pipes_anillo, i, n);
			}	
		}
		
	}

    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
    
    /* COMPLETAR */
	int envio = write(pipe_hijo[PIPE_WRITE], &buffer,sizeof(int));

	int lectura_hijo;
	int ree = read(pipe_hijo[PIPE_READ], &lectura_hijo, sizeof(int));

	printf("%d\n", lectura_hijo);

	return 0;
}	
