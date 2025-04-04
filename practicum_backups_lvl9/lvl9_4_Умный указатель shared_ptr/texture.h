#pragma once
#include "common.h"

class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
    }

    Size GetSize() const {
        return GetImageSize(image_); // Получаем размеры текстуры
    }

    char GetPixelColor(Point p) const {
        // Проверяем, находятся ли координаты в пределах текстуры
        if (static_cast<size_t>(p.y) >= image_.size() || 
            static_cast<size_t>(p.x) >= image_[0].size()) {
            return '.'; // Возвращаем символ точки, если координаты выходят за пределы
        }
        return image_[p.y][p.x]; // Возвращаем цвет пикселя
    }

private:
    Image image_;
};
