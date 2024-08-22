#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait>
#include <signal.h>

// la idea es poder desde un proceso bloquear al otro con SIGSTOP, escribir PING/PONG y luego SIGCONT para que el otro haga lo mismo
