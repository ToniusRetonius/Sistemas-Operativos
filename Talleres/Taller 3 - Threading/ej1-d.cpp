#include <thread>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <mutex>

#define CANT_THREADS 10

using namespace std;

mutex mtx;

void f(int i){
    // acá implementamos el lock() == como alternativa podemos usa lock_guard<mutex> lock(mtx); esto nos permite no poner el unlock() ya que con el destructor del thread se unlockea
    //mtx.lock();
    lock_guard<mutex> lock(mtx);
    printf("Hola! Soy el thread : %i \n", i);
    // espera de 200
    this_thread::sleep_for(200ms);
    printf("Chau saludos! Soy el thread : %i \n", i);
    // acǽ liberamos
    //mtx.unlock();
}

int main() {
    // vector de threads
    vector<thread> threads;

    for (int i = 0; i < CANT_THREADS; i++)
    {
        threads.emplace_back(f,i);
        sleep(0.2);
    }

    for (auto &t : threads)
    {
        t.join();
    }
    
    return 0;
}