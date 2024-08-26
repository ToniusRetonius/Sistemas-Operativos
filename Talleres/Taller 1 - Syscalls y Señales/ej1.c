#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

// las globales
int N, K, J;

void sigterm_handler(int signal){
	printf("Recibí SIGTERM\n");

	// generador de num random entre 0 y N
    int random = rand() % N;

	printf("Mi número random es: %d\n", random);
	
	// si es = al maldito
	if (random == J)
	{
		printf("Estas son mis últimas palabras\n");
		exit(0);
	}
	
}

void hijo(){
	// si le llega sigterm va al handler
	signal(SIGTERM, sigterm_handler);
	// si no, espera
	while(1){}
}

int main(int argc, const char* argv[]){
	// recibimos 3 argumentos :
	printf("Ingrese un valor válido para la cantidad de procesos: ");
	scanf("%d",&N);
	printf("Ingrese un valor válido de rondas: ");
	scanf("%d",&K);
	printf("Ingrese un número ☠ ☠ MALDITO ☠ ☠  ");
	scanf("%d",&J);

	// n no puede ser mayor a 10 y j no puede ser mayor que n ni menor a 0
	if(N > 10 || J > N || J < 0) exit(EXIT_FAILURE);

	// en pids nos guardamos los pids para poder después avisar uno por uno
	int pids[N];
	
	// creamos los n- hijos
	for (int i = 0; i < N; i++)
	{	
		// forkeamos
		pid_t pid = fork();

		// almacenamos en pids
		pids[i] = pid;
		
		// si no se creó correctamente
		if (pid == -1)exit(EXIT_FAILURE);
		
		// si se creó correctamente
		else if(pid == 0) hijo();
	}
	
	// k-veces la señal sigterm
	for (int m = 0; m < K; m++)
	{
		// a cada uno de sus hijos
		for (int indice = 0; indice < N; indice++)
		{
			// buscamos el pid
			kill(pids[m], SIGTERM);
			
			// esperamos 1 segundo
			sleep(1);
		}
		
	}

	// ahora hay que capturar aquellos que están running, y mandarles sigkill para eliminarlos y luego frenar al padre.
	
}
