#include <memory>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <cstdlib>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;
    
    explicit RawMemory(size_t capacity)
        : capacity_(capacity) {
        data_ = capacity_ > 0 ? static_cast<T*>(operator new(capacity_ * sizeof(T))) : nullptr;
    }
    
    ~RawMemory() {
        operator delete(data_);
    }
    
    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory&) = delete;
    
    RawMemory(RawMemory&& other) noexcept {
        Swap(other);
    }
    
    RawMemory& operator=(RawMemory&& other) noexcept {
        if (this != &other) {
            Swap(other);
        }
        return *this;
    }
    
    void Swap(RawMemory& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
    }
    
    const T* GetAddress() const noexcept {
        return data_;
    }
    
    T* GetAddress() noexcept {
        return data_;
    }
    
    size_t Capacity() const noexcept {
        return capacity_;
    }
    
    T& operator[](size_t index) {
        return data_[index];
    }
    
    const T& operator[](size_t index) const {
        return data_[index];
    }
    
private:
    T* data_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector {
public:
    Vector() = default;
    
    explicit Vector(size_t size)
        : data_(size) {
        size_ = size;
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }
    
    ~Vector() {
        DestroyN(data_.GetAddress(), size_);
    }
    
    Vector(const Vector& other)
        : data_(other.size_) {
        size_ = other.size_;
        std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
    }
    
    Vector(Vector&& other) noexcept {
        Swap(other);
    }
    
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            if (other.size_ > data_.Capacity()) {
                Vector tmp(other);
                Swap(tmp);
            } else {
                for (size_t i = 0; i < size_ && i < other.size_; ++i) {
                    data_[i] = other.data_[i];
                }
                if (size_ < other.size_) {
                    std::uninitialized_copy_n(other.data_.GetAddress() + size_, 
                                            other.size_ - size_, 
                                            data_.GetAddress() + size_);
                } else if (size_ > other.size_) {
                    DestroyN(data_.GetAddress() + other.size_, size_ - other.size_);
                }
                size_ = other.size_;
            }
        }
        return *this;
    }
    
    Vector& operator=(Vector&& rhs) noexcept {
        if (this != &rhs) {
            Swap(rhs);
        }
        return *this;
    }
    
    void Swap(Vector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }
    
    size_t Size() const noexcept { return size_; }
    size_t Capacity() const noexcept { return data_.Capacity(); }
    
    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    
    void Reserve(size_t new_capacity) {
        if (new_capacity > data_.Capacity()) {
            RawMemory<T> new_data(new_capacity);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            DestroyN(data_.GetAddress(), size_);
            data_.Swap(new_data);
        }
    }
    
    void Resize(size_t new_size) {
        if (new_size > size_) {
            Reserve(new_size);
            std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
        } else if (new_size < size_) {
            DestroyN(data_.GetAddress() + new_size, size_ - new_size);
        }
        size_ = new_size;
    }
    
    void PushBack(const T& value) {
        EmplaceBack(value);
    }
    
    void PushBack(T&& value) {
        EmplaceBack(std::move(value));
    }
    
    void PopBack() {
        if (size_ > 0) {
            data_[size_ - 1].~T();
            --size_;
        }
    }
    
    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            new (new_data.GetAddress() + size_) T(std::forward<Args>(args)...);
            
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            
            DestroyN(data_.GetAddress(), size_);
            data_.Swap(new_data);
            ++size_;
            return data_[size_ - 1];
        }
        
        new (data_.GetAddress() + size_) T(std::forward<Args>(args)...);
        ++size_;
        return data_[size_ - 1];
    }
    
private:
    static void DestroyN(T* buf, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            buf[i].~T();
        }
    }
    
    RawMemory<T> data_;
    size_t size_ = 0;
};