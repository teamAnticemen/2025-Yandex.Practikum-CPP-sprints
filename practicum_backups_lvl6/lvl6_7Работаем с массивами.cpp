#include <cassert>
#include <vector>

template <typename T>
void ReverseArray(T* start, size_t size) {
    if (size <= 1 || start == nullptr) {
        return; // ������ �� ������ ��� ������� ��� ��������������� �������
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

    vector<int> empty; // ������ � ������ ��������
    ReverseArray(empty.data(), empty.size());
    assert(empty == vector<int>{}); // ��������, ��� ������ �������� ������

    vector<int> single = {42}; // ������ � �������� �� ������ ��������
    ReverseArray(single.data(), single.size());
    assert(single == vector<int>{42}); // ��������, ��� ������ �������� ����������

    return 0;
}