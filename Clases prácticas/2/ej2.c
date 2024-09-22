#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

enum {READ, WRITE};

// la idea de este ejercicio es conectar los extremos del pipe

void ejecutar_hijo_1(int pipe_fd[]){
	// pasa que tenemos que cerrar este pipe del extremo de lectura pues no lo va a leer
	close(pipe_fd[READ]);
	// queremos que ls escriba en la punta de escritura y por ello ...
	dup2(pipe_fd[WRITE], STDIN_FILENO);
	execlp("ls","ls", "-al", NULL);
}

void ejecutar_hijo_2(int pipe_fd){
	// nos interesa cerrar el pipe heredado porque no lo va a escribir wc
	close(pipe_fd[WRITE]);
	// queremos que escriba en la punta de lectura
	dup2(pipe_fd[READ], STDOUT_FILENO);
	execlp("wc", "wc", "-l", NULL);
}

int main(int argc, char const* argv[]){
	int pipe_fd[2];
	pipe(pipe_fd);

	if(fork() == 0) ejecutar_hijo_1(pipe_fd);
	if(fork() == 0) ejecutar_hijo_2(pipe_fd);

	// cuantos procesos tienen acceso a esta punta de escritura? 3
	// forkeo 2 veces
	// hijo 2 close, hijo 1 ls, cierra .. pero a wc nunca le llega el end-of-file ...
	// quién tiene esa ref ? el padre .. entonces
	close(pipe_fd[WRITE]);

	wait(NULL);
	wait(NULL);

	return 0;
}
