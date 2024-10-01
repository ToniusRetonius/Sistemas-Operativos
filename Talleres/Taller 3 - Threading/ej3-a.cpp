#include <thread>
#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>

#define N 100
#define THREAD_COUNT 5

using namespace std;

// el int atómico que marca el lugar a partir del cual hay que modificar
atomic_int pos(0);
// esto es 
vector<int> p_por_thread(THREAD_COUNT);


void multiplicar_concurrente(iterator v1_begin, iterator v1_end, iterator v2_begin, iterator v2_end, iterator v3_begin, iterator v3_end, int step){
    int i;
    while ((i == pos++) < N)
    {
        v3[i] = v1[i] * v2[i];
        p_por_thread[t_i]++;
        // continuar
    }
    
    
}

void verify(vector<int> v1, vector<int> v2, vector<int> v3) {
    cout << "v1 = [" << v1[0] << ", " << v1[1] << ", " << ", ..., " << v1[N - 2] << ", " << v1[N - 1] << "]" << endl;
    cout << "v2 = [" << v2[0] << ", " << v2[1] << ", " << ", ..., " << v2[N - 2] << ", " << v2[N - 1] << "]" << endl;
    cout << "v3 = [" << v3[0] << ", " << v3[1] << ", " << ", ..., " << v3[N - 2] << ", " << v3[N - 1] << "]" << endl;

    for (int i = 0; i < N; ++i) {
        assert(v3[i] == v1[i] * v2[i]);
    }
}

int main() {
    vector<int> v1(N);
    vector<int> v2(N);
    iota(v1.begin(), v1.end(), 1);
    iota(v2.begin(), v2.end(), N + 1);

    vector<int> v3(N);
    
    // vamos entonces a declarar el vector de threads
    vector<thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        // 100 / 5 == pasos de a 20 para mover los índices
        int step = N / THREAD_COUNT;
        // inicio =  manejo de ínidce
        int start = i * step;
        // final = nro de paso + start
        int end = step + start;

        // creamos los threads, le pasamos la función y sus parámetros que es donde arranca, termina y el índice dado que queremos restringir cuántos valores modifica cada thread
        threads.emplace_back(multiplicar_concurrente, v1.begin() + start, v1.end() + end,v2.begin() + start, v2.end() + end, v3.begin() + start, v3.end() + end, step);

        // si queremos que cada thread modifique cualq cosa usamos 
        // threads.emplace_back(multiplicar_concurrente, ref(v1), ref(v2), ref(v3), )
    }
    

    verify(v1, v2, v3);

    return 0;
}