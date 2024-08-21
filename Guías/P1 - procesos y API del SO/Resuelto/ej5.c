#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const*argv[]){
	pid_t pid = fork();
	if (pid == 0){
		// HOMERO
		write(1, "Soy Homero\n", 11);
		sleep(2);
		pid_t pid_b = fork();
		if (pid_b == 0){
			write(1, "Soy Bart\n", 9);
			exit(EXIT_SUCCESS);
		}
		pid_t pid_l = fork();
		if (pid_l == 0){
			write(1, "Soy Lisa\n",10);
			exit(EXIT_SUCCESS);
		}
		pid_t pid_m = fork();
		if (pid_m == 0){
			write(1, "Soy Maggie\n",11);
			exit(EXIT_SUCCESS);
		}
		wait_for_child(pid_b);
		wait_for_child(pid_l);
		wait_for_child(pid_m);
		exit(EXIT_SUCCESS);
	} else {
		write(1,"Soy Abraham\n",12);
		sleep(2);
		wait_for_child(pid);
		exit(EXIT_SUCCESS);
	}
}
