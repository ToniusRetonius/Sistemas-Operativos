#include <thread>
#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>

#define N 100
#define THREAD_COUNT 5

using namespace std;

template<typename T>
class vectorWithConcurrentDotProduct : public vector<T> {
public:
    vectorWithConcurrentDotProduct(int t, int tCount) : vector<T>(t), tCount(tCount) {
    }

    int operator*(vectorWithConcurrentDotProduct<T> v) {
        assert(this->size() == v.size());

        atomic_int res(0);
        atomic_int pos(0);
        vector<thread> threads;

        for (int i = 0; i < tCount; ++i) {
            threads.emplace_back([&]() {
                int j;
                while ((j = pos++) < N) {
                    res += (*this)[j] * v[j];
                }
            });
        }

        for (auto &t: threads) {
            t.join();
        }
        return res;
    }

private:
    int tCount;
};

int main() {
    vectorWithConcurrentDotProduct<int> v1(N, THREAD_COUNT);
    vectorWithConcurrentDotProduct<int> v2(N, THREAD_COUNT);
    iota(v1.begin(), v1.end(), 1);
    iota(v2.begin(), v2.end(), N + 1);

    cout << "El resultado es " << v1 * v2 << endl;
    return 0;
}