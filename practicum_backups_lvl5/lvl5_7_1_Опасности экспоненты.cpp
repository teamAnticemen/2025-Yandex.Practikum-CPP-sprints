#include <cstdint>
#include <iostream>

using namespace std;

// упростите эту экспоненциальную функцию,
// реализовав линейный алгоритм
int64_t T(int i) {
    int64_t a0 = 0, a1 = 0, a2 = 1;
    if (i <= 1) {
        return 0;
    }
    if (i == 2) {
        return 1;
    }
    int64_t result = 0;
    for(int j = 3; j <= i; j++)
    {
        result = a0 + a1 + a2;
        a0 = a1;
        a1 = a2;
        a2 = result;
        //cout << "\n"s << a0 << " "s << a1 << " "s << a2 << " <> "s <<endl;//debug
    }
    return a2; 
}

int main() {
    int i;

    while (true) {
        cout << "Enter index: "s;
        //for(int j = 0; j < 100; j++) cout << j << " -->"s << T(j) << endl;//debug
        if (!(cin >> i)) {
            break;
        }

        cout << "Ti = "s << T(i) << endl;
    }
}