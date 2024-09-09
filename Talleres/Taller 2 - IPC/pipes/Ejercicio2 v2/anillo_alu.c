#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}

// el hijo distinguido recibe el nro c, la cantidad de procesos en el anillo, la referencia la pipe con el cual se comunica con el padre y todas las referencias a todos los pipes y su índice para saber cómo comunicarse
void hijo_distinguido(int c, int n, int pipe_padre_hijo[], int pipes_anillo[][2], int indice){
	// genera el nro secreto, debe ser estrictamente mayor a 'c'
	int nro_secreto = generate_random_number();
	while (nro_secreto < c) nro_secreto = generate_random_number();

	printf("El nro secreto es %i \n", nro_secreto);

	// incia el ciclo, sumando 1 a c, escribiendo en el pipe
	int primer_escritura = c + 1;
	int e = write(pipes_anillo[(indice + 1) % n][PIPE_WRITE], &primer_escritura, sizeof(int));

	// ------------------------------ close -----------------------------
	for (int j = 0; j < n; j++)
	{
		// cerramos todas las refencias heredadas que no usamos
		int sucesor = (indice + 1) % n;
		int precesor = (indice - 1) % n;
		if (j != indice && j != sucesor && j != precesor)
		{
			close(pipes_anillo[j][PIPE_WRITE]);
			close(pipes_anillo[j][PIPE_READ]);
		}
	}
	// ------------------------------ close -----------------------------

	// una vez iniciado el ciclo, ya podemos quedarnos esperando la lectura del proceso antecesor
	while (1)
	{
		// lectura del antecesor
		int lectura;
		int l = read(pipes_anillo[(indice - 1) % n][PIPE_READ], &lectura, sizeof(int));

		// chequeamos si es mayor o igual al nro secreto
		if (lectura >= nro_secreto)
		{
			// envía al proceso padre el valor obtenido
			int e = write(pipe_padre_hijo[PIPE_WRITE], &lectura, sizeof(int));
	
			// termina
			exit(0);
		}

		// caso contrario :  inicia otra ronda
		lectura++;
		int e = write(pipes_anillo[(indice + 1) % n][PIPE_WRITE], &lectura, sizeof(int)); 
	}
}

// el hijo comun recibe los pipes, su indice y el valor de n para mantener el anillo
void hijo_comun(int n, int indice, int pipes_anillo[][2]){
	
	// ------------------------------ close -----------------------------
	for (int j = 0; j < n; j++)
	{
		// cerramos todas las refencias heredadas que no usamos
		int sucesor = (indice + 1) % n;
		int precesor = (indice - 1) % n;
		if (j != indice && j != sucesor && j != precesor)
		{
			close(pipes_anillo[j][PIPE_WRITE]);
			close(pipes_anillo[j][PIPE_READ]);
		}
	}
	// ------------------------------ close -----------------------------
	
	// siempre espera que le escriban un valor en el pipe para poder aumentarlo y escribir en el sucesor
	while (1)
	{
		// lectura
		int lectura;
		int l = read(pipes_anillo[(indice - 1) % n][PIPE_READ], &lectura, sizeof(int));

		// incrementa
		lectura++;

		// reescribe
		int e = write(pipes_anillo[(indice + 1) % n][PIPE_WRITE], &lectura, sizeof(int));
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
	
	// incializamos pipe padre-hijo
	int pipe_padre_hijo[2];
	pipe(pipe_padre_hijo);

	// incializamos el arreglo de pipes
	int pipes_anillo[n][2];
	for (int i = 0; i < n; i++)
	{
		pipe(pipes_anillo[i]);
	}

	// array de pids
	int pids[n];

	// creación de procesos
	for (int j = 0; j < n; j++)
	{
		pid_t pid = fork();
		
		// capturamos el pid
		pids[j] = pid;
		
		if(pid == -1)exit(1);
		else if (pid == 0)
		{
			// si justo es el índice correspondiente al hijo que inicializa el anillo
			if (j == start)
			{
				hijo_distinguido(buffer, n, pipe_padre_hijo, pipes_anillo, j);
			}
			// si es un hijo cualquiera
			else
			{
				hijo_comun(n, j, pipes_anillo);
			}
			
		}
		
	}
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
    
    /* COMPLETAR */
    // cuando el hijo distinguido recibe el valor que es mayor o igual a 'c'
	// como es bloqueante la lectura esta, el padre se queda esperando
	int lectura;
	int l = read(pipe_padre_hijo[PIPE_READ], &lectura, sizeof(int));
	printf("%i \n", lectura);

	// tenemos que matar a todos los hijos
	for (int i = 0; i < n; i++)
	{
		// matamos a todos los hijos
		kill(pids[i], SIGKILL);
	}
	
	return 0;
}
