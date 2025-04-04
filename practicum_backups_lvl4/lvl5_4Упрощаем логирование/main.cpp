#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>
#include "log_duration.h"

using namespace std;

vector<int> ReverseVector(const vector<int>& source_vector) {
    vector<int> res;
    for (int i : source_vector) {
        res.insert(res.begin(), i);
    }

    return res;
}

int CountPops(const vector<int>& source_vector, int begin, int end) {
    int res = 0;

    for (int i = begin; i < end; ++i) {
        if (source_vector[i]) {
            ++res;
        }
    }

    return res;
}

void AppendRandom(vector<int>& v, int n) {
    for (int i = 0; i < n; ++i) {
        // получаем случайное число с помощью функции rand.
        // с помощью (rand() % 2) получим целое число в диапазоне 0..1.
        // в C++ имеются более современные генераторы случайных чисел,
        // но в данном уроке не будем их касаться
        v.push_back(rand() % 2);
    }
}

void Operate() {
    LogDuration total("Total"s);
    vector<int> random_bits;

    // операция << для целых чисел это сдвиг всех бит в двоичной
    // записи числа. Запишем с её помощью число 2 в степени 17 (131072)
    static const int N = 1 << 17;

    // заполним вектор случайными числами 0 и 1
    {
    LogDuration random("Append random"s);
    AppendRandom(random_bits, N);
    }
    
    // перевернём вектор задом наперёд
    {
    LogDuration random("Reverse"s);
    vector<int> reversed_bits = ReverseVector(random_bits);
    
    
    // посчитаем процент единиц на начальных отрезках вектора
    {
        LogDuration random("Counting"s);
        for (int i = 1, step = 1; i <= N; i += step, step *= 2) {
            double rate = CountPops(reversed_bits, 0, i) * 100. / i;
            cout << "After "s << i << " bits we found "s << rate << "% pops"s << endl;
        }
    }
    }
}

int main() {
    // название переменной не играет роли
    LogDuration guard("Total sleeping"s);

    {
        LogDuration guard("Sleeping 1 sec"s);
        this_thread::sleep_for(chrono::seconds(1));
    }

    {
        LogDuration guard("Sleeping 2 sec"s);
        this_thread::sleep_for(chrono::seconds(2));
    }
}