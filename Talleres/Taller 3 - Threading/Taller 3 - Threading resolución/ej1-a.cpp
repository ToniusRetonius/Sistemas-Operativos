#include <thread>
#include <iostream>
#include <unistd.h>

#define CANT_THREADS 10

using namespace std;

void f() {
    this_thread::sleep_for(500ms);
    cout << "Hola! Soy un thread" << endl;
}

int main() {
    for (int i = 0; i < CANT_THREADS; i++) {
        new thread(f);
    }

    sleep(1);
    return 0;
}