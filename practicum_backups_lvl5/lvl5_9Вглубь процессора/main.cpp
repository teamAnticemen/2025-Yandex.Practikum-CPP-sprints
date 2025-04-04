#include "log_duration.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace std;

vector<float> ComputeAvgTemp(const vector<vector<float>>& measures) {
    // место для вашего решения
    vector<float> averageVec;
    if(measures.empty()) return averageVec;//проверка пустого вектора
    
    averageVec.resize(measures[0].size());//Основной вектор для записи значений
    vector<size_t> indexVec(measures[0].size());//Доп вектор для усреднения после записи основного
    
    //cout << "averageVec.size() -> "s << averageVec.size() << endl;
    //cout << "measures[0].size() -> "s << measures[0].size() << endl;
    for(auto one : measures)
    {
        for (auto it = one.size(); it > 0; --it) {
            //cout << "it 1-> "s << one[it-1] << endl;
            if(one[it-1] > 0.0f)
            {
              averageVec[it-1] += one[it-1];
              indexVec[it-1]++; 
              //cout << "it 1-> "s << one[it] << endl;
              //cout << "averageVec[it] 1-> "s << averageVec[it] << endl;
              //cout << "indexVec[it] 1-> "s << indexVec[it] << endl;
            }
        }
        //cout << "-----"s << endl;
    }
    //cout << "averageVec.size() -> "s << averageVec.size() << endl;
    //cout << "averageVec[0] -> "s << averageVec[0] << endl;
            //cout << "indexVec[0] -> "s << indexVec[0] << endl;

    //cout << "-AAAAA-"s << endl;
    for(auto it = averageVec.size(); it > 0; --it)
    {
       //cout << "averageVec -> "s << averageVec[it] << endl;
       //cout << "indexVec -> "s << indexVec[it] << endl;
        if(indexVec[it-1] == 0) averageVec[it-1] = 0;
        else      averageVec[it-1] /= indexVec[it-1];
    }
    
    return averageVec;
}

vector<float> GetRandomVector(int size) {
    static mt19937 engine;
    uniform_real_distribution<float> d(-500, -100);

    vector<float> res(size);
    for (int i = 0; i < size; ++i) {
        res[i] = d(engine);
    }

    return res;
}

int main() {
    vector<vector<float>> data;
    data.reserve(10);

    for (int i = 0; i < 10; ++i) {
        data.push_back(GetRandomVector(15));
    }

    vector<float> avg;
    {
        LOG_DURATION("ComputeAvgTemp"s);
        avg = ComputeAvgTemp(data);
    }

    cout << "Total mean: "s << accumulate(avg.begin(), avg.end(), 0.f) / avg.size() << endl;
}