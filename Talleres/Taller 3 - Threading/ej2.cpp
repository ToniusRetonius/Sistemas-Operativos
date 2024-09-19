#include <thread>
#include <iostream>
#include <semaphore>

using namespace std;

// inicializar semǽforo
// g++ --std=c++20 ej2.cpp -o ej2 para compilar y que no se rompa todo
binary_semaphore sem1(0);
binary_semaphore sem2(0);

// wait() se llama acquire() y signal() es realease()

void f1() {
    // hago algo
    // ...
    // aviso que terminé
    sem1.release();

    // necesito ejecutar otra cosa, entonces quiero adquirir
    sem2.acquire();
    // ...

}

void f2() {
    // hago algo
    // ...
    // aviso que terminé
    sem2.release();

    // necesito ejecutar otra cosa, entonces quiero adquirir
    sem1.acquire();
    // ...
}

int main() {
    // lanzamos los threads
    thread t1(f1);
    thread t2(f2);

    // esperamos que terminen
    t1.join();
    t2.join();

    return 0;
}