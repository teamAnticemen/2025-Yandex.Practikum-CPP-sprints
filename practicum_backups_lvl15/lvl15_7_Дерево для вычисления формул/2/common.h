#pragma once

#include <stdexcept>
#include <ostream>

// Описывает ошибки, которые могут возникнуть при вычислении формулы.
class FormulaError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
    
    friend std::ostream& operator<<(std::ostream& output, const FormulaError& fe) {
        return output << "#ARITHM!";
    }
};

// Исключение, выбрасываемое при попытке задать синтаксически некорректную
// формулу
class FormulaException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};