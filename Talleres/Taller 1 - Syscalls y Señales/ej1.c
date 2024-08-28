#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

// LAS GLOBALES NO SON MEMORIA COMPARTIDA!!!!

// las globales
int N, K, J;

// los inicializamos en 10 porque por consigna no es mayor a 10 N
// en pids nos guardamos los pids para poder después avisar uno por uno
int pids[10];

// en pids_vivos guardamos 1 si está vivo y 0 si está muerto (inician todos muertos)
int pids_vivos[10] = {0};


void sigterm_handler(int signal){
	// los print son para saber si está funcionando
	printf("Recibí SIGTERM\n");

	// generador de num random entre 0 y N
    int random = rand() % N;

	printf("Mi número random es: %d\n", random);
	
	// si es = al maldito
	if (random == J)
	{
		// imprime
		printf("Estas son mis últimas palabras\n");
		// termina y esto manda un SICHLD al padre que captura en un handler
		exit(0);
	}
	
}

void sigchld_handler(int signal){
	//
	printf("Murió un niño!\n");

	// capturamos el pid del hijo que acaba de morir
	int* pid_;
	pid_t pid_hijo = wait(pid_);

	// recorremos pids ,lo encontramos y luego con el índice modificamos el valor en pids_vivos
	for (int i = 0; i < N; i++)
	{
		if (pids[i] == pid_hijo)
		{
			// seteamos en cero su vida
			pids_vivos[i] = 0;
		}
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

	// creamos los n- hijos
	for (int i = 0; i < N; i++)
	{	
		// forkeamos
		pid_t pid = fork();

		// almacenamos en pids
		pids[i] = pid;

		// avisamos que está vivo
		pids_vivos[i] = 1;

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

			signal(SIGCHLD, sigchld_handler);
		}	
	}


	// el padre busca en pids_vivos aquellos vivos, y los mata
	for (int l = 0; l < N; l++)
	{
		// si está vivo ... 
		if (pids_vivos[l] == 1)
		{
			// anunciamos ganadores falla!
			printf("Ganador: %d\n",pids[l]);
			// lo invitamos a morir
			kill(pids[l], SIGKILL);
		}
		
	}

	// termina su ejecución;
	exit(0);
}
