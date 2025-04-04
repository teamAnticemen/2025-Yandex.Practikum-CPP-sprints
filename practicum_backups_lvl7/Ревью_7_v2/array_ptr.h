#pragma once
#include <memory>
#include <utility>
#include <cassert>

template<typename T>
class ArrayPtr {
public:
    ArrayPtr() noexcept : data_(nullptr), size_(0) {}

    explicit ArrayPtr(size_t size) : data_(std::make_unique<T[]>(size)), size_(size) {}

    ArrayPtr(const ArrayPtr& other) = delete; // Запрет копирования
    ArrayPtr& operator=(const ArrayPtr& other) = delete; // Запрет копирования

    ArrayPtr(ArrayPtr&& other) noexcept 
        : data_(std::move(other.data_)), size_(other.size_) {
        other.size_ = 0; // Обнуляем размер перемещенного объекта
    }

    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            size_ = other.size_;
            other.size_ = 0; // Обнуляем размер перемещенного объекта
        }
        return *this;
    }

    T& operator[](size_t index) {
        assert(index < size_ && "Index out of range");
        return data_[index];
    }

    const T& operator[](size_t index) const {
        assert(index < size_ && "Index out of range");
        return data_[index];
    }

    size_t Size() const noexcept {
        return size_;
    }

    T* Get() noexcept {
        return data_.get();
    }

    const T* Get() const noexcept { // Добавляем константную версию
        return data_.get();
    }

private:
    std::unique_ptr<T[]> data_;
    size_t size_;
};