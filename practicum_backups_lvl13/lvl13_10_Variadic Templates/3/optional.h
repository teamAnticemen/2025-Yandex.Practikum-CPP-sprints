#pragma once
#include <stdexcept>
#include <utility>
#include <type_traits>

class BadOptionalAccess : public std::exception {
public:
    const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() noexcept = default;

    Optional(const T& value) : has_value_(true) {
        new (&storage_) T(value);
    }

    Optional(T&& value) : has_value_(true) {
        new (&storage_) T(std::move(value));
    }

    Optional(const Optional& other) : has_value_(other.has_value_) {
        if (other.has_value_) {
            new (&storage_) T(other.Value());
        }
    }

    Optional(Optional&& other) noexcept : has_value_(other.has_value_) {
        if (other.has_value_) {
            new (&storage_) T(std::move(other.Value()));
            other.has_value_ = false; // Не вызываем Reset() чтобы избежать лишнего вызова деструктора
        }
    }

    ~Optional() {
        if (has_value_) {
            Value().~T();
        }
    }

    Optional& operator=(const Optional& other) {
        if (this != &other) {
            if (other.has_value_) {
                if (has_value_) {
                    Value() = other.Value();
                } else {
                    new (&storage_) T(other.Value());
                    has_value_ = true;
                }
            } else {
                Reset();
            }
        }
        return *this;
    }

    Optional& operator=(Optional&& other) noexcept {
        if (this != &other) {
            if (other.has_value_) {
                if (has_value_) {
                    Value() = std::move(other.Value());
                } else {
                    new (&storage_) T(std::move(other.Value()));
                    has_value_ = true;
                }
                other.has_value_ = false; // Не вызываем Reset() чтобы избежать лишнего вызова деструктора
            } else {
                Reset();
            }
        }
        return *this;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, Optional>>>
    Optional& operator=(U&& value) {
        if (has_value_) {
            Value() = std::forward<U>(value);
        } else {
            new (&storage_) T(std::forward<U>(value));
            has_value_ = true;
        }
        return *this;
    }

    bool HasValue() const noexcept {
        return has_value_;
    }

    T& Value() & {
        if (!has_value_) throw BadOptionalAccess();
        return *reinterpret_cast<T*>(&storage_);
    }

    const T& Value() const & {
        if (!has_value_) throw BadOptionalAccess();
        return *reinterpret_cast<const T*>(&storage_);
    }

    T&& Value() && {
        if (!has_value_) throw BadOptionalAccess();
        return std::move(*reinterpret_cast<T*>(&storage_));
    }

    const T&& Value() const && {
        if (!has_value_) throw BadOptionalAccess();
        return std::move(*reinterpret_cast<const T*>(&storage_));
    }

    T* operator->() {
        return &Value();
    }

    const T* operator->() const {
        return &Value();
    }

    T& operator*() & {
        return Value();
    }

    const T& operator*() const & {
        return Value();
    }

    T&& operator*() && {
        return std::move(Value());
    }

    const T&& operator*() const && {
        return std::move(Value());
    }

    void Reset() noexcept {
        if (has_value_) {
            Value().~T();
            has_value_ = false;
        }
    }

    template <typename... Args>
    void Emplace(Args&&... args) {
        Reset();
        new (&storage_) T(std::forward<Args>(args)...);
        has_value_ = true;
    }

private:
    alignas(T) unsigned char storage_[sizeof(T)];
    bool has_value_ = false;
};