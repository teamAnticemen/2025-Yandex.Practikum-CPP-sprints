#include "read_input_functions.h"
#include <iostream>
#include <string>

std::string Read_Input::ReadLine() {
    std::string s;
    getline(std::cin, s);
    return s;
}

int Read_Input::ReadLineWithNumber() {
    int result;
    std::cin >> result;
    ReadLine();
    return result;
}