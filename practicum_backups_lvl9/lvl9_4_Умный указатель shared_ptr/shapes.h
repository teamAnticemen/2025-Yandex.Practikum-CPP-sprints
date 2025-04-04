// shapes.h
#pragma once
#include "texture.h"
#include <iostream>
#include <memory>

enum class ShapeType { RECTANGLE, ELLIPSE };

class Shape {
public:
    explicit Shape(ShapeType type)
        : type_(type), position_{0, 0}, size_{0, 0} {}

    void SetPosition(Point pos) {
        position_ = pos;
    }

    void SetSize(Size size) {
        size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = std::move(texture);
    }

   void Draw(Image& image) const {
    // Проверяем, задана ли текстура и имеет ли она ненулевые размеры
    if (!texture_ || texture_->GetSize().width == 0 || texture_->GetSize().height == 0) {
        // Если текстура не задана или пустая, заполняем изображение точками
        for (int y = 0; y < size_.height; ++y) {
            for (int x = 0; x < size_.width; ++x) {
                Point p = {position_.x + x, position_.y + y};

                // Проверка на выход за пределы изображения
                if (p.x < 0 || p.x >= static_cast<int>(image[0].size()) || 
                    p.y < 0 || p.y >= static_cast<int>(image.size())) {
                    continue; // Пропускаем пиксели, выходящие за пределы изображения
                }

                image[p.y][p.x] = '.'; // Заполняем пиксель точкой
            }
        }
        return; // Завершаем выполнение, так как текстура не задана
    }
    
    // Получаем размеры текстуры
    Size texture_size = texture_->GetSize();

    // Корректируем размеры фигуры, если они выходят за пределы изображения
    int max_width = std::min(size_.width, static_cast<int>(image[0].size()) - position_.x);
    int max_height = std::min(size_.height, static_cast<int>(image.size()) - position_.y);
       
        // Если текстура задана, рисуем её
    for (int y = 0; y < max_height; ++y) {
        for (int x = 0; x < max_width; ++x) {
            Point p = {position_.x + x, position_.y + y};

            // Проверка на выход за пределы изображения
            if (p.x < 0 || p.x >= static_cast<int>(image[0].size()) || 
                p.y < 0 || p.y >= static_cast<int>(image.size())) {
                continue; // Пропускаем пиксели, выходящие за пределы изображения
            }

            // Проверяем, чтобы размеры фигуры не превышали размеры текстуры
            int draw_x = x % texture_size.width;
            int draw_y = y % texture_size.height;

            // Получаем цвет пикселя из текстуры
            char pixel_color = texture_->GetPixelColor({draw_x, draw_y});

            // Проверка на выход за пределы изображения при рисовании фигуры
            if (p.x >= 0 && p.x < static_cast<int>(image[0].size()) && 
                p.y >= 0 && p.y < static_cast<int>(image.size())) {
                if (type_ == ShapeType::RECTANGLE) {
                    // Если размеры текстуры меньше размеров поля, заполняем недостающие пиксели точками
                if (texture_size.width < size_.width || texture_size.height < size_.height) {
                    if (x >= texture_size.width || y >= texture_size.height) {
                        image[p.y][p.x] = '.'; // Заполняем точкой, если текстура меньше
                        continue; // Пропускаем дальнейшую обработку для этого пикселя
                    }
                }
                    image[p.y][p.x] = pixel_color; // Рисуем пиксель
                } else if (type_ == ShapeType::ELLIPSE) {
                    // Используем глобальную функцию IsPointInEllipse
                    if (::IsPointInEllipse({x, y}, size_)) { // Обратите внимание на :: перед именем функции
                        // Если размеры текстуры меньше размеров поля, заполняем недостающие пиксели точками
                if (texture_size.width < size_.width || texture_size.height < size_.height) {
                    if (x >= texture_size.width || y >= texture_size.height) {
                        image[p.y][p.x] = '.'; // Заполняем точкой, если текстура меньше
                        continue; // Пропускаем дальнейшую обработку для этого пикселя
                    }
                }
                        image[p.y][p.x] = pixel_color; // Рисуем пиксель
                    }
                }
            }
        }
    }
}


private:
    ShapeType type_;
    Point position_;
    Size size_;
    std::shared_ptr<Texture> texture_;

    bool IsPointInRectangle(Point p) const {
        return p.x >= position_.x && p.x < position_.x + size_.width &&
               p.y >= position_.y && p.y < position_.y + size_.height;
    }
};