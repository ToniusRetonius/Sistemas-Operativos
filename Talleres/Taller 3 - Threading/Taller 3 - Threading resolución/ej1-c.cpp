#include <thread>
#include <iostream>
#include <vector>

#define CANT_THREADS 10

using namespace std;

void f(int i) {
    this_thread::sleep_for(1000ms * i);
    cout << "Hola! Soy el thread: " << i << endl;
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