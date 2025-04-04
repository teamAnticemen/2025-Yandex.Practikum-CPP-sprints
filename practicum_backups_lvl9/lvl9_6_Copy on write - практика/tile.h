#pragma once
#include <array>
#include <cassert>
#include "geom.h"

class Tile {
public:
    constexpr static int SIZE = 8;

    Tile(char color = ' ') noexcept {
        pixels_.fill(color);  // Заполняем тайл указанным цветом
        assert(instance_count_ >= 0);
        ++instance_count_;
    }

    Tile(const Tile& other) : pixels_(other.pixels_) {
        assert(instance_count_ >= 0);
        ++instance_count_;
    }

    ~Tile() {
        --instance_count_;
        assert(instance_count_ >= 0);
    }

    void SetPixel(Point p, char color) noexcept {
        if (p.x >= 0 && p.x < SIZE && p.y >= 0 && p.y < SIZE) {
            pixels_[p.y * SIZE + p.x] = color;  // Устанавливаем цвет пикселя
        }
    }

    char GetPixel(Point p) const noexcept {
        if (p.x >= 0 && p.x < SIZE && p.y >= 0 && p.y < SIZE) {
            return pixels_[p.y * SIZE + p.x];  // Возвращаем цвет пикселя
        }
        return ' ';  // Возвращаем пробел, если координаты выходят за пределы
    }

    static int GetInstanceCount() noexcept {
        return instance_count_;
    }

private:
    inline static int instance_count_ = 0;
    std::array<char, SIZE * SIZE> pixels_;  // Хранение пикселей тайла
};