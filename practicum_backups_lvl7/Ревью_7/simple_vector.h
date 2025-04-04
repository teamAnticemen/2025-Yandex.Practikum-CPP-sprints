#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>

struct ReserveProxyObj {
    explicit ReserveProxyObj(size_t capacity) : capacity_to_reserve(capacity) {}
    size_t capacity_to_reserve;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    SimpleVector() noexcept : data_(nullptr), size_(0), capacity_(0) {}

    explicit SimpleVector(size_t size) noexcept : SimpleVector() {
        Resize(size);
    }

    SimpleVector(size_t size, const Type& value) : SimpleVector(size) {
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
        data_ = new Type[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i]; // Задаем копии
        }
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& other) noexcept 
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Оператор присваивания
    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            data_ = new Type[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i]; // Копируем значения
            }
        }
        return *this;
    }

    // Оператор перемещения
    SimpleVector& operator=(SimpleVector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~SimpleVector() {
        delete[] data_;
    }

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

    void PushBack(Type value) noexcept {
        if (size_ >= capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_] = std::move(value);
        ++size_;
    }

    void PopBack() noexcept {
       assert(size_ > 0);
       --size_;
       // Необходимо сбросить значение только если это необходимо
       // data_[size_] = Type(); // Это можно сделать, если требуется сбросить значение
   }
   
    // Вариант с перемещением
    Type* Insert(Type* position, Type value) noexcept {
    size_t index = position - data_;

    // Проверка текущей емкости
    if (size_ >= capacity_) {
        Reserve(capacity_ == 0 ? 1 : capacity_ * 2); // Увеличение емкости
    }

    // Сдвиг элементов вправо, начиная с позиции index
    for (size_t i = size_; i > index; --i) {
        data_[i] = std::move(data_[i - 1]);
    }
    
    // Вставка нового элемента
    data_[index] = std::move(value);
    ++size_;

    return &data_[index]; // Возвращаем указатель на вставленный элемент
    }
    
    void Insert(size_t index, Type value) {
        assert(index <= size_ && "Index out of range");

        // Увеличиваем емкость, если это нужно
        if (size_ >= capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        // Сдвигаем элементы вправо, начиная с позиции index
        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }

        // Вставляем новый элемент
        data_[index] = std::move(value);
        ++size_; // Увеличиваем размер
    }
   
    // Удаление элементов
    Type* Erase(Type* position) noexcept {
        size_t index = position - data_;
        for (size_t i = index; i < size_ - 1; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;
        return data_ + index; // Возвращаем итератор на следующий элемент
    }

    void Erase(size_t index) {
       assert(index < size_ && "Index out of range");
       for (size_t i = index; i < size_ - 1; ++i) {
           data_[i] = std::move(data_[i + 1]); // Сдвиг элементов влево
       }
       --size_;
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
        return data_;
    }

    // Константная версия метода begin() для работы с const объектами
    const Type* begin() const noexcept {
        return data_;
    }

    // Неконстантная версия метода end()
    Type* end() noexcept {
        return data_ + size_;
    }

    // Константная версия метода end()
    const Type* end() const noexcept {
        return data_ + size_;
    }

    const Type* cbegin() const noexcept {
        return data_;
    }

    const Type* cend() const noexcept {
        return data_ + size_;
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
        return data_;
    }

    const Type* data() const noexcept {
        return data_;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(data_, other.data_);
    }
    
    bool operator==(const SimpleVector& other) const noexcept {
        if (size_ != other.size_) return false;
        for (size_t i = 0; i < size_; ++i) {
            if (!(data_[i] == other.data_[i])) return false;
        }
        return true;
    }

    bool operator!=(const SimpleVector& other) const { return !(*this == other); }

    bool operator<(const SimpleVector& other) const {
        size_t min_size = std::min(size_, other.size_);
        for (size_t i = 0; i < min_size; ++i) {
            if (data_[i] < other.data_[i]) return true;
            if (data_[i] > other.data_[i]) return false;
        }
        return size_ < other.size_; // If equal, compare sizes
    }

    bool operator>(const SimpleVector& other) const { return other < *this; }
    bool operator<=(const SimpleVector& other) const { return !(*this > other); }
    bool operator>=(const SimpleVector& other) const { return !(*this < other); }
    
    // Конструктор, принимающий ReserveProxyObj
    SimpleVector(ReserveProxyObj reserve) : data_(nullptr), size_(0), capacity_(0) {
        Reserve(reserve.capacity_to_reserve);
    }
    
    void Reserve(size_t new_capacity) noexcept {
        if (new_capacity > capacity_) {
            Type* new_data = new Type[new_capacity];
            if (data_) {
                for (size_t i = 0; i < size_; ++i) {
                    new_data[i] = std::move(data_[i]);
                }
                delete[] data_;
            }
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }
        
        size_t Size() const {
        return size_;
    }
private:
    Type* data_;
    size_t size_;
    size_t capacity_;
};