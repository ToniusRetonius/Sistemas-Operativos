#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// dado es un archivo que simula un dado
#include "dado.h"

// definimos las macros, enum nos permite enumerar desde 0
enum {READ, WRITE}
enum {LESTER, ELIZA}

// que sea var global tenemos referencia de los file descriptors que cree
// también se puede pasar por argumento, como alternativa
int pipes[2][2]

void lester(){
	//tiro el dado
	int dado = tirar_dado();
	// informamos con el pipe a Humberto el valor del dado
	// recordar que write es write( file descriptor, buffer, size)
	// en el file descr ... el pipe de LESTER, escribimos lo que nos dice el dado y el tamaño del dato que hay que escrbir es .. 	
	write(pipes[LESTER][WRITE], &dado, sizeof(dado));
	// me tengo que ir porque rompo todo
	exit(EXIT_SUCCESS);
}

void eliza(){
	int dado = tirar_dado();
	write(pipes[ELIZA][WRITE], &dado, sizeof(dado));
	// el cero en exit es análogo al success
	exit(0)
}

int main (int argc, const char* argv[]){
	// creamos los pipes
	pipe(pipes[LESTER]);
	pipe(pipes[ELIZA]);

	// creamos a Lester
	pid_t pid_lester = fork();
	if(pid_lester == 0) lester();
	// creamos a ELiza
	pid_t pid_eliza = fork();
	if(pid_eliza == 0) eliza();

	// recibo el dato del dado de  Lester
	int dado_lester;
	read(pipes[LESTER][READ], &dado_lester, sizeof(dado_lester));

	// recibo el dato de Eliza
	int dado_eliza;
	read(pipes[ELIZA][READ], &dado_eliza, sizeof(dado_eliza));

	// anuncio al ganador
	if (dado_lester < dado_eliza){
		printf("Ganó ELiza\n");
	} else if (dado_lester > dado_eliza){
		printf("Ganó Lester\n");
	} else {
		printf("Empate");
	}
}
