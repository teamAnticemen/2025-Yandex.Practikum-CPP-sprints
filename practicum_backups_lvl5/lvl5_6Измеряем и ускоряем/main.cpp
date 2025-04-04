#include "log_duration.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

vector<int> ReverseVector(const vector<int>& source_vector) {
    LOG_DURATION("Naive"s);
    vector<int> res;

    for (int i : source_vector) {
        res.insert(res.begin(), i);
    }
    return res;
}

vector<int> ReverseVector2(const vector<int>& source_vector) {
    LOG_DURATION("Good"s);
    vector<int> res;

    // будем проходить source_vector задом наперёд
    // с помощью обратного итератора
    for (auto iterator = source_vector.rbegin(); iterator != source_vector.rend(); ++iterator) {
        res.push_back(*iterator);
    }
    return res;
}

vector<int> ReverseVector3(const vector<int>& source_vector) {
    LOG_DURATION("Best"s);    
    return {source_vector.rbegin(), source_vector.rend()};
}

vector<int> ReverseVector4(const vector<int>& source_vector) {
    LOG_DURATION("Your"s);
    //vector<int> res(source_vector.size());

    return {source_vector.rbegin(), source_vector.rend()};
}

void Operate() {
    vector<int> rand_vector;
    int n;

    cin >> n;
    rand_vector.reserve(n);

    for (int i = 0; i < n; ++i) {
        rand_vector.push_back(rand());
    }
    
    if(n <= 100000)
    {
        ReverseVector(rand_vector);
        ReverseVector2(rand_vector);
    }
    else
    {
        ReverseVector2(rand_vector);
        ReverseVector3(rand_vector);
        ReverseVector4(rand_vector);
    }
    // код измерения тут
}

int main() {
    Operate();
    return 0;
}