#include <cassert>
#include <vector>

template <typename T>
void ReverseArray(T* start, size_t size) {
    if (size <= 1 || start == nullptr) {
        return; // Ничего не делаем для пустого или одноэлементного массива
    }
    
    T* end = start + size - 1;
    while (start < end) {
        std::swap(*start, *end);
        ++start;
        --end;
    }
}

int main() {
    using namespace std;
    
    vector<int> v = {1, 2};
    ReverseArray(v.data(), v.size());
    assert(v == (vector<int>{2, 1}));

    vector<int> empty; // Пример с пустым массивом
    ReverseArray(empty.data(), empty.size());
    assert(empty == vector<int>{}); // Проверка, что массив остается пустым

    vector<int> single = {42}; // Пример с массивом из одного элемента
    ReverseArray(single.data(), single.size());
    assert(single == vector<int>{42}); // Проверка, что массив остается неизменным

    return 0;
}