#include "read_input_functions.h"

string Read_input::ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int Read_input::ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}