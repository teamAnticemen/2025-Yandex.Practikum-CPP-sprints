#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;
    Optional(const T& value);
    Optional(T&& value);
    Optional(const Optional& other);
    Optional(Optional&& other);

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs);
    Optional& operator=(const Optional& rhs);
    Optional& operator=(Optional&& rhs);

    ~Optional();

    bool HasValue() const;

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value();
    const T& Value() const;

    void Reset();

private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
};#include <stdexcept>
#include <utility>
#include <type_traits>

// Исключение этого типа должно генерироваться при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() : is_initialized_(false) {}

    Optional(const T& value) {
        new (data_) T(value);
        is_initialized_ = true;
    }

    Optional(T&& value) {
        new (data_) T(std::move(value));
        is_initialized_ = true;
    }

    Optional(const Optional& other) {
        if (other.is_initialized_) {
            new (data_) T(*other);
            is_initialized_ = true;
        } else {
            is_initialized_ = false;
        }
    }

    Optional(Optional&& other) noexcept {
        if (other.is_initialized_) {
            new (data_) T(std::move(*other));
            is_initialized_ = true;
            other.is_initialized_ = false;  // Помечаем other как пустой
        } else {
            is_initialized_ = false;
        }
    }

    Optional& operator=(const T& value) {
        if (is_initialized_) {
            **this = value;
        } else {
            new (data_) T(value);
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(T&& value) {
        if (is_initialized_) {
            **this = std::move(value);
        } else {
            new (data_) T(std::move(value));
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(const Optional& other) {
        if (this != &other) {
            if (other.is_initialized_) {
                if (is_initialized_) {
                    **this = *other;
                } else {
                    new (data_) T(*other);
                    is_initialized_ = true;
                }
            } else {
                Reset();
            }
        }
        return *this;
    }

    Optional& operator=(Optional&& other) noexcept {
        if (this != &other) {
            if (other.is_initialized_) {
                if (is_initialized_) {
                    **this = std::move(*other);
                } else {
                    new (data_) T(std::move(*other));
                    is_initialized_ = true;
                }
                other.is_initialized_ = false;  // Помечаем other как пустой
            } else {
                Reset();
            }
        }
        return *this;
    }

    ~Optional() {
        if (is_initialized_) {
            reinterpret_cast<T*>(data_)->~T();
        }
    }

    bool HasValue() const {
        return is_initialized_;
    }

    T& operator*() {
        return *reinterpret_cast<T*>(data_);
    }

    const T& operator*() const {
        return *reinterpret_cast<const T*>(data_);
    }

    T* operator->() {
        return reinterpret_cast<T*>(data_);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(data_);
    }

    T& Value() {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return **this;
    }

    const T& Value() const {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return **this;
    }

    void Reset() {
        if (is_initialized_) {
            reinterpret_cast<T*>(data_)->~T();
            is_initialized_ = false;
        }
    }

private:
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
};