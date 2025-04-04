#pragma once

#include <optional>
#include <string_view>
#include <memory>

class BiMap {
public:
    BiMap();
    BiMap(const BiMap& other); // Конструктор копирования
    BiMap(BiMap&& other) noexcept; // Конструктор перемещения
    BiMap& operator=(const BiMap& other); // Оператор копирующего присваивания
    BiMap& operator=(BiMap&& other) noexcept; // Оператор перемещающего присваивания
    ~BiMap(); // Деструктор

    bool Add(std::string_view key, std::string_view value);
    std::optional<std::string_view> FindValue(std::string_view key) const noexcept;
    std::optional<std::string_view> FindKey(std::string_view value) const noexcept;

private:
    struct Impl; // Предварительное объявление структуры Impl
    std::unique_ptr<Impl> pImpl; // Указатель на реализацию
};
