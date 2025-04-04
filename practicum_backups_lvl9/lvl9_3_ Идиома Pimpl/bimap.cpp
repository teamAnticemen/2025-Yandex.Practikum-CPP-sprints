#include "bimap.h"
#include <unordered_map>
#include <deque>
#include <stdexcept>
#include <string>

struct BiMap::Impl {
    std::unordered_map<std::string_view, std::string_view> key_to_value;
    std::unordered_map<std::string_view, std::string_view> value_to_key;
    std::deque<std::string> storage; // Для хранения строк

    Impl() = default;

    // Конструктор копирования
    Impl(const Impl& other) {
        storage = other.storage; // Копируем строки
        key_to_value = other.key_to_value; // Копируем мапы
        value_to_key = other.value_to_key; // Копируем мапы
    }
};

BiMap::BiMap() : pImpl(std::make_unique<Impl>()) {}

BiMap::BiMap(const BiMap& other) : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

BiMap::BiMap(BiMap&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr; // Указываем на пустой указатель
}

BiMap& BiMap::operator=(const BiMap& other) {
    if (this == &other) return *this; // Проверка на самоприсваивание
    pImpl = std::make_unique<Impl>(*other.pImpl); // Копируем
    return *this;
}

BiMap& BiMap::operator=(BiMap&& other) noexcept {
    if (this == &other) return *this; // Проверка на самоприсваивание
    pImpl = std::move(other.pImpl); // Перемещаем
    other.pImpl = nullptr; // Указываем на пустой указатель
    return *this;
}

BiMap::~BiMap() = default;

bool BiMap::Add(std::string_view key, std::string_view value) {
    if (pImpl->key_to_value.count(key) || pImpl->value_to_key.count(value)) {
        return false; // Ключ или значение уже существуют
    }

    try {
        // Сохраняем строки в deque
        pImpl->storage.emplace_back(key);
        std::string_view key_sv = pImpl->storage.back();
        pImpl->storage.emplace_back(value);
        std::string_view value_sv = pImpl->storage.back();

        // Добавляем в карты
        pImpl->key_to_value[key_sv] = value_sv;
        pImpl->value_to_key[value_sv] = key_sv;
    } catch (...) {
        // Откат изменений в случае исключения
        pImpl->storage.pop_back(); // Удаляем value
        pImpl->storage.pop_back(); // Удаляем key
        throw; // Перевыбрасываем исключение
    }

    return true;
}

std::optional<std::string_view> BiMap::FindValue(std::string_view key) const noexcept {
    auto it = pImpl->key_to_value.find(key);
    if (it != pImpl->key_to_value.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string_view> BiMap::FindKey(std::string_view value) const noexcept {
    auto it = pImpl->value_to_key.find(value);
    if (it != pImpl->value_to_key.end()) {
        return it->second;
    }
    return std::nullopt;
}