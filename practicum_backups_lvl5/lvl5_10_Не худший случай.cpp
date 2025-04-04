#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

int EffectiveCount(const vector<int>& v, int n, int i) {
    // Ожидаемое количество чисел, не превышающих i
    int64_t expected = static_cast<int64_t>(v.size()) * (i + 1) / (n + 1);
    int threshold = static_cast<int>(log2(v.size()));

    // Если ожидаемое значение меньше или равно log2(v.size()), используем find_if
    if (expected <= threshold) {
        cout << "Using find_if" << endl;
        auto it = find_if(v.begin(), v.end(), [i](int value) { return value > i; });
        return distance(v.begin(), it); // Количество чисел, не превышающих i
    } else {
        // Иначе, используем upper_bound
        cout << "Using upper_bound" << endl;
        auto it = upper_bound(v.begin(), v.end(), i);
        return distance(v.begin(), it); // Количество чисел, не превышающих i
    }
}

int main() {
    static const int NUMBERS = 1'000'000;
    static const int MAX = 1'000'000'000;

    mt19937 r;
    uniform_int_distribution<int> uniform_dist(0, MAX);

    vector<int> nums;
    for (int i = 0; i < NUMBERS; ++i) {
        int random_number = uniform_dist(r);
        nums.push_back(random_number);
    }
    sort(nums.begin(), nums.end());

    int i;
    cin >> i; // Ввод числа i для поиска
    int result = EffectiveCount(nums, MAX, i);
    cout << "Total numbers before "s << i << ": "s << result << endl;

    return 0;
}