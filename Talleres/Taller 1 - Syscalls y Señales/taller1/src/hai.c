#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void handler_sigurg(){
	printf("ya va!\n");
}

void hijo(){
	signal(SIGURG, handler_sigurg);
	while (1)
	{
		/* code */
	}
	
}


int main(int argc, char* argv[]) {
	// recibe parámetros
	char* parametro = argv[1];

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
	}
	// con este esperamos la última respuesta
	sleep(1);

	kill(pid, SIGKILL);
	exit(0);

	return 0;
}
