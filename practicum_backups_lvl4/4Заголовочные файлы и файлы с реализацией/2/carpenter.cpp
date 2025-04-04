#include "carpenter.h"
//#include "square_calculation.h"

// Определите методы класса здесь
int Carpenter::CalcShelves(Wall& wal)
{
    return CalcSquare(wal.GetHeight(), wal.GetWidth()) / 2;
}