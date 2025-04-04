#include <iostream>
#include <string>

using namespace std;

template <typename F>
string BruteForceInternal(F check, const string& begin, int n) {
    // ���� ����� ������ �������� 5, ��������� �
    if (n == 5) {
        return check(begin) ? begin : "";
    }
    
    // ������ ��������, ������� �� ����� ������������
    const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // ����� �������� �� ������ ����� ��������

    // ���������� ��� ��������� �������
    for (char c : characters) {
        // ����������� �����, ���������� ������ � ����������� �����
        string result = BruteForceInternal(check, begin + c, n + 1);
        
        // ���� ������ �� �����, ���������� �, ���������� ������� �� �����
        if (!result.empty()) {
            return result;
        }
    }

    // ���� �� ���� ������ �� �������, ���������� ������ ������
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