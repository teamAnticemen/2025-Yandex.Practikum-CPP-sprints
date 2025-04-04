#pragma once
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include "array_ptr.h" 

struct ReserveProxyObj {
    explicit ReserveProxyObj(size_t capacity) : capacity_to_reserve(capacity) {}
    size_t capacity_to_reserve;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

// Предварительное объявление класса SimpleVector
template<typename T>
class SimpleVector;

// Определение оператора
template<typename T>
bool operator>=(const SimpleVector<T>& lhs, const SimpleVector<T>& rhs) {
    return !(lhs < rhs);
}

template <typename Type>
class SimpleVector {
public:
    SimpleVector() noexcept : data_(), size_(0), capacity_(0) {}

    explicit SimpleVector(size_t size) noexcept : SimpleVector() {
        Resize(size);
    }

    SimpleVector(size_t size, const Type& value) : SimpleVector() {
        Reserve(size);
        size_ = size;
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = value;
        }
    }

    SimpleVector(std::initializer_list<Type> init) : SimpleVector() {
        Reserve(init.size());
        size_ = 0;
        for (const auto& element : init) {
            PushBack(element);
        }
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = ArrayPtr<Type>(other.capacity_);
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i]; // Задаем копии
        }
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& other) noexcept 
        : data_(std::move(other.data_)), size_(other.size_), capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Оператор присваивания
    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            data_ = ArrayPtr<Type>(other.capacity_);
            size_ = other.size_;
            capacity_ = other.capacity_;
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i]; // Копируем значения
            }
        }
        return *this;
    }

    // Оператор перемещения
    SimpleVector& operator=(SimpleVector&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~SimpleVector() = default; // Умный указатель сам управляет памятью

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }
    
    void Clear() noexcept {
        size_ = 0;  // Сбрасываем только размер, память все еще выделена.
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_ && "Index out of range");
        return data_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_ && "Index out of range");
        return data_[index];
    }

    void PushBack(const Type& value) { // Копирующий PushBack
        if (size_ >= capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_] = value; // Копируем значение
        ++size_;
    }

    void PushBack(Type&& value) noexcept { // Перемещающий PushBack
        if (size_ >= capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_] = std::move(value); // Перемещаем значение
        ++size_;
    }

    void PopBack() noexcept {
        assert(size_ > 0);
        --size_;
    }
   
    // Определяем псевдоним типа для итераторов
    using Iterator = Type*;
    using ConstIterator = const Type*;

    Iterator Insert(Iterator position, const Type& value) { // Копирующий Insert
        assert(position >= data_.Get() && position <= data_.Get() + size_);

        size_t index = position - data_.Get();

        // Проверка текущей емкости
        if (size_ >= capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2); // Увеличение емкости
        }

        // Сдвиг элементов вправо, начиная с позиции index
        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }

        // Вставка нового элемента
        data_[index] = value; // Копируем значение
        ++size_;

        return &data_[index]; // Возвращаем указатель на вставленный элемент
    }

    Iterator Insert(Iterator position, Type&& value) noexcept { // Перемещающий Insert
        assert(position >= data_.Get() && position <= data_.Get() + size_);

        size_t index = position - data_.Get();

        // Проверка текущей емкости
        if (size_ >= capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2); // Увеличение емкости
        }

        // Сдвиг элементов вправо, начиная с позиции index
        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }

        // Вставка нового элемента
        data_[index] = std::move(value); // Перемещаем значение
        ++size_;

        return &data_[index]; // Возвращаем указатель на вставленный элемент
    }

    void Insert(size_t index, const Type& value) { // Копирующий вариант
        assert(index <= size_ && "Index out of range");
        Insert(data_.Get() + index, value); 
    }

    void Insert(size_t index, Type&& value) noexcept { // Перемещающий вариант
        assert(index <= size_ && "Index out of range");
        Insert(data_.Get() + index, std::move(value));
    }
   
    // Удаление элементов
    Type* Erase(Type* position) noexcept {
        assert(position >= data_.Get() && position < data_.Get() + size_ && "Position out of range");
        size_t index = position - data_.Get();
        for (size_t i = index; i < size_ - 1; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;
        return data_.Get() + index; // Возвращаем итератор на следующий элемент
    }
   
    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    void Resize(size_t new_size) noexcept {
        Reserve(new_size);
        for (size_t i = size_; i < new_size; ++i) {
            data_[i] = Type(); // Инициализированные значения по умолчанию
        }
        size_ = new_size;
    }

    // Неконстантная версия метода begin()
    Type* begin() noexcept {
        return data_.Get();
    }

    // Константная версия метода begin() для работы с const объектами
    const Type* begin() const noexcept {
        return data_.Get();
    }

    // Неконстантная версия метода end()
    Type* end() noexcept {
        return data_.Get() + size_;
    }

    // Константная версия метода end()
    const Type* end() const noexcept {
        return data_.Get() + size_;
    }

    const Type* cbegin() const noexcept {
        return data_.Get();
    }

    const Type* cend() const noexcept {
        return data_.Get() + size_;
    }

    Type& front() noexcept {
        assert(size_ > 0);
        return data_[0];
    }

    Type& back() noexcept {
        assert(size_ > 0);
        return data_[size_ - 1];
    }

    const Type& front() const noexcept {
        assert(size_ > 0);
        return data_[0];
    }

    const Type& back() const noexcept {
        assert(size_ > 0);
        return data_[size_ - 1];
    }

    Type* data() noexcept {
        return data_.Get();
    }

    const Type* data() const noexcept {
        return data_.Get();
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(data_, other.data_);
    }
    
    bool operator==(const SimpleVector& other) const noexcept {
        if (size_ != other.size_) return false;
        return std::equal(data_.Get(), data_.Get() + size_, other.data_.Get());
    }

    bool operator!=(const SimpleVector& other) const { return !(*this == other); }

    bool operator<(const SimpleVector& other) const {
        return std::lexicographical_compare(data_.Get(), data_.Get() + size_, other.data_.Get(), other.data_.Get() + other.size_);
    }
    bool operator>(const SimpleVector& other) const { return other < *this; }
    bool operator<=(const SimpleVector& other) const { return !(*this > other); }
        
    // Конструктор, принимающий ReserveProxyObj
    SimpleVector(ReserveProxyObj reserve) : data_(), size_(0), capacity_(0) {
        Reserve(reserve.capacity_to_reserve);
    }
    
    void Reserve(size_t new_capacity) noexcept {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_data(new_capacity);
            if (data_.Get()) {
                for (size_t i = 0; i < size_; ++i) {
                    new_data[i] = std::move(data_[i]);
                }
            }
            data_ = std::move(new_data);
            capacity_ = new_capacity;
        }
    }
        
    size_t Size() const {
        return size_;
    }

private:
    ArrayPtr<Type> data_; // Используем ArrayPtr вместо сырого указателя
    size_t size_;
    size_t capacity_;
};