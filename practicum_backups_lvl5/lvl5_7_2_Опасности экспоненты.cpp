#include <iostream>
#include <string>

using namespace std;

template <typename F>
string BruteForceInternal(F check, const string& begin, int n) {
    // Если длина строки достигла 5, проверяем её
    if (n == 5) {
        return check(begin) ? begin : "";
    }
    
    // Массив символов, которые мы будем использовать
    const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Можно изменить на другой набор символов

    // Перебираем все возможные символы
    for (char c : characters) {
        // Рекурсивный вызов, прибавляем символ и увеличиваем длину
        string result = BruteForceInternal(check, begin + c, n + 1);
        
        // Если строка не пуста, возвращаем её, дальнейший перебор не нужен
        if (!result.empty()) {
            return result;
        }
    }

    // Если ни одна строка не подошла, возвращаем пустую строку
    return "";
}

template <typename F>
string BruteForce(F check) {
    string result = BruteForceInternal(check, "", 0);
    return result.empty() ? "Not found" : result;
}

int main() {
    string pass = "ARTUR";
    auto check = [pass](const string& s) {
        return s == pass;
    };
    cout << BruteForce(check) << endl;
    return 0;
}