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
    LOG_DURATION("Total"s);
    vector<int> random_bits;

    // операция << для целых чисел это сдвиг всех бит в двоичной
    // записи числа. Запишем с её помощью число 2 в степени 17 (131072)
    static const int N = 1 << 17;

    // заполним вектор случайными числами 0 и 1
    {
    LOG_DURATION("Append random"s);
    AppendRandom(random_bits, N);
    }
    
    // перевернём вектор задом наперёд
    vector<int> reversed_bits;
    {
    LOG_DURATION("Reverse"s);
    reversed_bits = ReverseVector(random_bits);
    }
    
    // посчитаем процент единиц на начальных отрезках вектора
    {
        LOG_DURATION("Counting"s);
        for (int i = 1, step = 1; i <= N; i += step, step *= 2) {
            // чтобы вычислить проценты, мы умножаем на литерал 100. типа double;
            // целочисленное значение функции CountPops при этом автоматически
            // преобразуется к double, как и i
            double rate = CountPops(reversed_bits, 0, i) * 100. / i;
            cout << "After "s << i << " bits we found "s << rate << "% pops"s
                 << endl;
        }
    }
}

//LOG_DURATION(int UNIQUE_VAR_NAME_PROFILE);
int main() {
    Operate();
    return 0;
}