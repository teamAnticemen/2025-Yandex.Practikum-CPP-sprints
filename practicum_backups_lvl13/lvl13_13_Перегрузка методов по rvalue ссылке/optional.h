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
    Optional() = default;

    Optional(const T& value) 
        : has_value_(true) {
        new (&storage_) T(value);
    }

    Optional(T&& value) 
        : has_value_(true) {
        new (&storage_) T(std::move(value));
    }

    Optional(const Optional& other) 
        : has_value_(other.has_value_) {
        if (has_value_) {
            new (&storage_) T(*other);
        }
    }

    Optional(Optional&& other) noexcept 
        : has_value_(other.has_value_) {
        if (has_value_) {
            new (&storage_) T(std::move(*other));
            other.has_value_ = false;
        }
    }

    ~Optional() {
        Reset();
    }

    Optional& operator=(const Optional& other) {
        if (this != &other) {
            if (other.has_value_) {
                if (has_value_) {
                    **this = *other;
                } else {
                    new (&storage_) T(*other);
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
                    **this = std::move(*other);
                } else {
                    new (&storage_) T(std::move(*other));
                    has_value_ = true;
                }
                other.has_value_ = false;
            } else {
                Reset();
            }
        }
        return *this;
    }

    template <typename U, typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, Optional>>>
    Optional& operator=(U&& value) {
        if (has_value_) {
            **this = std::forward<U>(value);
        } else {
            new (&storage_) T(std::forward<U>(value));
            has_value_ = true;
        }
        return *this;
    }

    void Reset() {
        if (has_value_) {
            reinterpret_cast<T*>(&storage_)->~T();
            has_value_ = false;
        }
    }

    bool HasValue() const noexcept {
        return has_value_;
    }

    explicit operator bool() const noexcept {
        return HasValue();
    }

    T& operator*() & {
        return *reinterpret_cast<T*>(&storage_);
    }

    const T& operator*() const & {
        return *reinterpret_cast<const T*>(&storage_);
    }

    T&& operator*() && {
        return std::move(*reinterpret_cast<T*>(&storage_));
    }

    T* operator->() {
        return reinterpret_cast<T*>(&storage_);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(&storage_);
    }

    T& Value() & {
        if (!has_value_) {
            throw BadOptionalAccess();
        }
        return **this;
    }

    const T& Value() const & {
        if (!has_value_) {
            throw BadOptionalAccess();
        }
        return **this;
    }

    T&& Value() && {
        if (!has_value_) {
            throw BadOptionalAccess();
        }
        return std::move(**this);
    }

    template <typename... Args>
    void Emplace(Args&&... args) {
        Reset();
        new (&storage_) T(std::forward<Args>(args)...);
        has_value_ = true;
    }

private:
    alignas(T) char storage_[sizeof(T)];
    bool has_value_ = false;
};