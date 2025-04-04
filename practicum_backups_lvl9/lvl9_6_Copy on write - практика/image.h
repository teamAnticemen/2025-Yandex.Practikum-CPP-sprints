#pragma once

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cassert>
#include "tile.h"
#include "cow.h"  // Подключение файла cow.h

class Image {
public:
    explicit Image(Size size, char color = ' ')
        : size_{size.width > 0 && size.height > 0 ? size
                                                  : throw std::out_of_range("Invalid image size")},
          size_in_tiles_{(size_.width + Tile::SIZE - 1) / Tile::SIZE,
                         (size_.height + Tile::SIZE - 1) / Tile::SIZE},
          tiles_(size_in_tiles_.width * size_in_tiles_.height, CoWTile{Tile{color}}) {
    }

    Size GetSize() const noexcept {
        return size_;
    }

    char GetPixel(Point p) const noexcept {
        if (p.x < 0 || p.x >= size_.width || p.y < 0 || p.y >= size_.height) {
            return ' ';
        }
        int tileIndex = GetTileIndex(p);
        Point tilePixel = {p.x % Tile::SIZE, p.y % Tile::SIZE};
        return (*tiles_[tileIndex]).GetPixel(tilePixel);  // Используем оператор разыменования
    }

    void SetPixel(Point p, char color) {
        if (p.x < 0 || p.x >= size_.width || p.y < 0 || p.y >= size_.height) {
            return;
        }
        int tileIndex = GetTileIndex(p);
        Point tilePixel = {p.x % Tile::SIZE, p.y % Tile::SIZE};
        tiles_[tileIndex].Write([&](Tile& tile) { tile.SetPixel(tilePixel, color); });  // Используем Write для изменения
    }

private:
    using CoWTile = CoW<Tile>;  // Определяем CoWTile как CoW<Tile>

    int GetTileIndex(Point p) const noexcept {
        assert((p.x >= 0) && (p.x < size_.width) && (p.y >= 0) && (p.y < size_.height));
        return (p.y / Tile::SIZE) * size_in_tiles_.width + (p.x / Tile::SIZE);
    }

    Size size_;                   // Размер изображения в пикселях
    Size size_in_tiles_;          // Размер изображения в тайлах
    std::vector<CoWTile> tiles_;  // Тайлы изображения (строка за строкой)
};

/**
 * Выводит в поток out изображение в виде символов.
 */
inline void Print(const Image& img, std::ostream& out) {
    const auto size = img.GetSize();
    for (int y = 0; y < size.height; ++y) {
        for (int x = 0; x < size.width; ++x) {
            out.put(img.GetPixel({x, y}));
        }
        out.put('\n');
    }
}

/**
 * Загружает изображение из pixels. Линии изображения разделяются символами \n.
 * Размеры картинки определяются по количеству переводов строки и самой длинной линии.
 */
inline Image LoadImage(const std::string& pixels) {
    std::istringstream s(pixels);
    Size size;
    std::string line;
    while (std::getline(s, line)) {
        size.width = std::max(size.width, static_cast<int>(line.length()));
        ++size.height;
    }

    Image img(size);

    s = std::istringstream(pixels);
    for (int y = 0; y < size.height; ++y) {
        if (!std::getline(s, line)) break;

        int x = 0;
        for (char ch : line) {
            img.SetPixel({x++, y}, ch);
        }
    }

    return img;
}