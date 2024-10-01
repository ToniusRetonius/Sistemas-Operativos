#include <thread>
#include <iostream>
#include <vector>

#define CANT_THREADS 10

using namespace std;

void f() {
    this_thread::sleep_for(500ms);
    cout << "Hola! " << "Soy un thread" << endl;
}

int main() {
    vector<thread> threads;

    for (int i = 0; i < CANT_THREADS; i++) {
        threads.emplace_back(f);
    }

    for (auto &t: threads) {
        // Opción 1
        t.join();
        // Opción 2
        //t.detach();
    }

    return 0;
}