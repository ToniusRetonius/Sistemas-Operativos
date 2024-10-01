#include <thread>
#include <iostream>
#include <semaphore.h>

using namespace std;

#define MSG_COUNT 3

sem_t sem1;
sem_t sem2;

void f1_a() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F1 (A)\n";
        this_thread::sleep_for(100ms);
    }
}

void f1_b() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F1 (B)\n";
        this_thread::sleep_for(200ms);
    }
}

void f2_a() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F2 (A)\n";
        this_thread::sleep_for(500ms);
    }
}

void f2_b() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F2 (B)\n";
        this_thread::sleep_for(10ms);
    }
}

void f1() {
    f1_a();
    sem_post(&sem1);
    sem_wait(&sem2);
    f1_b();
}

void f2() {
    f2_a();
    sem_post(&sem2);
    sem_wait(&sem1);
    f2_b();
}

int main() {
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);

    thread t1(f1);
    thread t2(f2);

    t1.join();
    t2.join();

    return 0;
}
