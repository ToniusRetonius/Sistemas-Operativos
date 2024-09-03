#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

typedef char*** matrix;


/*
	Explicacion del argumento progs:

	Corresponde con una matriz donde cada fila representa el iesimo programa pasado
	dividido por el pipe. En cada fila esta el nombre del programa 
	en su primera posicion y los parametros en las posiciones restantes.

	Ejemplo: 'du -h /home | head -n 10 | sort -rh'
	
	Representacion:
	[
		['du', '-h', '/home'],
		['head', '-n', '10'],
		['sort', '-rh']
	]

*/
// execvp ( prog[0], prog, NULL) chequear null

void hijo(int i, int pipes[][2],int n, char** instruccion){
	// cerramos todos los pipes que no se usan!
	for (size_t j = 0; j < n; j++)
	{
		if (j == i){
			// para el hijo en el que estoy parado me interesa cerrar el de escritura del pipe anterior porque no voy a escribir ahí
			// y tampoco voy a leer del que me conecta con el siguiente
			// mantengo leer del pipe que me conecta con mi anterior y escribir en mi siguiente
			close(pipes[i-1][PIPE_WRITE]);
			close(pipes[i+1][PIPE_READ]);
		}
		// cerramos todas las referencias heredadas que no usamos
		close(pipes[j][PIPE_WRITE]);
		close(pipes[j][PIPE_READ]);
	}

	if(i == 0){
		// redirección STDIN al pipe del primer hijo
		dup2(pipes[i][PIPE_WRITE], STD_OUTPUT);

	}else if(i == n-1)
	{
		dup2(STD_OUTPUT, pipes[i-1][PIPE_WRITE]);
	}else
	{
		dup2(pipes[i-1][PIPE_READ], STD_INPUT);
		dup2(pipes[i+1][PIPE_WRITE], STD_OUTPUT);
	}
	
	execvp(instruccion[0], instruccion);
}


	
static int run(matrix progs, size_t count)
{	
	int r, status;
	

	//TODO: Pensar cuantos pipes necesito.
	int pipes_hijos[count][2];
	for (int i = 0; i < count; i++)
	{
		// inicializamos los pipes
		pipe(pipes_hijos[i]);
	}
	//Rservo memoria para el arreglo de pids
	//TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * count);

	//TODO: Pensar cuantos procesos necesito
	// creación de hijos
	for (int i = 0; i < count; i++)
	{
		pid_t pid = fork();

		if (pid == -1)exit(1);
		else if (pid == 0)
		{
			// codigo hijo
			hijo(i, pipes_hijos, count, progs[i]);
		}
		children[i] = pid;
	}

	// cerrar los pipes del padre 
	for (int i = 0; i < count; i++)
	{
		close(pipes_hijos[i][PIPE_READ]);
		close(pipes_hijos[i][PIPE_WRITE]);
	}
	
	//TODO: Para cada proceso hijo:
			//1. Redireccionar los file descriptors adecuados al proceso
			//2. Ejecutar el programa correspondiente

	//Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	matrix programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
