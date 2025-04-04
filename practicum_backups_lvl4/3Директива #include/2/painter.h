#include "wall.h"

class Painter {
public:
    // Напишите класс Painter
    void Paint(Wall& wall, Wall::Color col) {
        
       wall.SetColor(col);        
    }
};