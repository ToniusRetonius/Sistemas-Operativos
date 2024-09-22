#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// este handler va a ser el que use el hijo cuando el padre le mande la señal
void pong_handler(int signal){
	printf("PONG\n");
}

int main(int argc, const char* argv[]){
	pid_t pid = fork();
	if (pid == -1)exit(EXIT_FAILURE);
	else if (pid == 0){
		// cuando reciba la señal 10, el handler hace eso que dice
		signal(10, pong_handler);
		// se implementa con un bucle infinito para que espere infinitamente las órdenes del padre
		while(1){}
	}
	else{
		// si no ponés este sleep, falla, por qué? ni idea
		sleep(1);
		// código del padre
		for(int i = 0; i < 3; i++){
			printf("PING\n");
			// le mandamos al hijo la señal 10 y esperamos a que haga lo que dice el handler
			kill(pid, 10);
			sleep(1);
		}
		// después de hacerlo 3 veces nos dicen que le preguntemos al usuario
		char* res[1];
		printf("Desea finalizar [Y/n] ? ");
		scanf("%s", *res);

		// mientras que el usuario ponga que no
		while(strcmp(*res,"n") == 0){
			// hacemos el ping pong
			for(int i = 0; i < 3; i++){
				printf("PING\n");
				kill(pid, 10);
				sleep(1);
			}
			// preguntamos de nuevo para actualizar res
			printf("Qué hacemos pe? [Y/n] ");
			// pisamos res con la nueva respuesta
			scanf("%s", *res);
		}
		// si llegamos acá es porque el usuario decidió salir del programa
		// matamos al hijo y salimos del programa
		kill(pid, SIGTERM);
		exit(0);
	}

}
