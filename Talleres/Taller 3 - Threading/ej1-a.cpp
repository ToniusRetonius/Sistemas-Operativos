#include <thread>
#include <iostream>
#include <unistd.h>
#include <vector>

#define CANT_THREADS 10

using namespace std;

void f(int i){
    // si modificamos este sleep de manera que podamos controlar la concurrencia para hacer la ejecución secuencial (antes tenía 500ms)
    this_thread::sleep_for(1000ms * i);
    // puts("Hola soy un Thread!\n");
    printf("Hola! Soy el thread : %i \n", i);
}

int main() {
    // vector de threads
    vector<thread> threads;

    for (int i = 0; i < CANT_THREADS; i++)
    {
        // thread t = thread ( f ); esto no funciona y aparentemente es por el compilador
        // solución 1 :
        // new thread(f);

        // solución 2 :
        threads.emplace_back(f,i);
    }
    // si le sacás el sleep este, no imprime nada, esto es porque termina la ejecución el que creo los threads
    //sleep(1);
    
    // aparece el método .join(), que lo que hace es una especie de wait que el creador del thread espera a que pase al estado JOINABLE que sucede cuando el thread finaliza su ejecución ( para ello, necesitamos guardar una referencia : normalmente un vector de threads)
    
    // en este caso hacemos la espera de cada thread
    for (auto &t : threads)
    {
        t.join();
        // para qué sirve detach ? La idea es que al hacer uso de recursos que no necesitan del proceso creador, dejamos al hilo correr sin la necesidad de que el programa padre se quede esperando. EJ : servidor que recibe muchas request, liberan al proceso padre y hacen lo que sea
        // t.detach();
    }
    
    return 0;
}