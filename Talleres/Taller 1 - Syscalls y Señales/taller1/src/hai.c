#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// globales
int veces = 5;
char* programa;
char* parametro;

void handler_sigurg(int signal){

	if (veces > 0)
	{
		printf("ya va!\n");
		veces--;
	}
	else
	{
		// captura pid del padre
		pid_t pid_padre = getppid();
		// le envía un SIGINT al padre que el reacciona al final
		kill(pid_padre, SIGINT);
	}
}

void handler_sigint(int signal){
	printf("Esto es SIGINT\n");
	// acá buscamos el otro programa con EXCECVP y nos vamos de la ejecución
	// acǽ debería tomar como segundo argumento aquello que le pasamos por terminal 
	execvp(programa, (char *[]){programa, parametro, NULL});
}

void hijo(){
	signal(SIGURG, handler_sigurg);
	while (1)
	{
	}
}


int main(int argc, char* argv[]) {
	// recibe parámetros
	programa = argv[1];
	parametro = argv[2];


	// clone
	pid_t pid = fork();
	if (pid == -1)exit(1);
	if (pid == 0)
	{
		hijo();
	}

	
	for (int i = 0; i < 5; i++)
	{
		// duerme 1 seg
		sleep(1);
		// el padre imprime sup!
		printf("sup!\n");
		// le manda la señal al hijo
		kill(pid, SIGURG);
		
		signal(SIGINT, handler_sigint);
	}
	// con este esperamos la última respuesta
	sleep(1);
	
	// le manda la última señal al hijo
	kill(pid, SIGURG);
	// lo esperamos
	sleep(1);
	// entramos a SIGINT
	signal(SIGINT, handler_sigint);
	

	kill(pid, SIGKILL);
	exit(0);

	return 0;
}
