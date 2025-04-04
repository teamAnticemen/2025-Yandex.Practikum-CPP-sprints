#include <algorithm>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

// функция возвращает true, если векторы из одинаковых элементов
// перепишите эту функцию, улучшив её асимптотическую сложность
bool TestPermut(const vector<int>& v1, const vector<int>& v2) {
    // если они разной длины, элементы заведомо разные
    if (v1.size() != v2.size()) {
        return false;
    }
    
    vector<int> a1, a2;    
    for (int i = 0; i < v1.size() ; i++) {
        a1.push_back(v1[i]);
        a2.push_back(v2[i]);        
    }

    sort(a1.begin(), a1.end());
    sort(a2.begin(), a2.end());
    for (int i = 0; i < v1.size() ; i++) {
        //cout << i << endl;
        if(a1[i] != a2[i]) return false;        
    }
    return true;
}

int main() {
    std::mt19937 g;

    int n;
    cin >> n;
    vector<int> v1, v2;
    v1.reserve(n);
    v2.reserve(n);

    for (int i = 0; i < n; ++i) {
        v1.push_back(rand());
        v2.push_back(rand());
    }

    // оба вектора случайны, вряд ли они совпадут
    cout << "Random vectors match? "s << flush;
    cout << (TestPermut(v1, v2) ? "Yes"s : "No"s) << endl;

    // делаем один перестановкой другого явным образом
    v2 = v1;
    shuffle(v2.begin(), v2.end(), g);
    cout << "Permuted vectors match? "s << flush;
    cout << (TestPermut(v1, v2) ? "Yes"s : "No"s) << endl;
}