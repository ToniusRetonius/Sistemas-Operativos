#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

#define CANT_THREADS 10

using namespace std;

mutex mtx;

void f(int i) {
    lock_guard<mutex> lock(mtx);
    cout << "Hola! Soy el thread: " << i << endl;
    this_thread::sleep_for(100ms);
    cout << "Chau! Saludos desde: " << i << endl;
}

int main() {
    vector<thread> threads;

    for (int i = 0; i < CANT_THREADS; i++) {
        threads.emplace_back(f, i);
    }

    for (auto &t: threads) {
        t.join();
    }

    return 0;
}