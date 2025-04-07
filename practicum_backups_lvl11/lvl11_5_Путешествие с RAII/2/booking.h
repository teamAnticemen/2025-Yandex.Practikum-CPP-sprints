#pragma once

#include <utility>  // Для std::move

namespace raii {

template <typename Provider>
class Booking {
public:
    using BookingId = typename Provider::BookingId;

    // Конструктор, принимающий провайдера и идентификатор бронирования
    Booking(Provider* provider, BookingId id)
        : provider_(provider), id_(id) {}

    // Запрещаем копирование бронирования
    Booking(const Booking&) = delete;
    Booking& operator=(const Booking&) = delete;

    // Разрешаем перемещение бронирования
    Booking(Booking&& other) noexcept
        : provider_(other.provider_), id_(other.id_) {
        other.provider_ = nullptr;  // Обнуляем указатель у перемещённого объекта
    }

    Booking& operator=(Booking&& other) noexcept {
        if (this != &other) {
            // Освобождаем текущее бронирование
            if (provider_) {
                provider_->CancelOrComplete(*this);
            }
            // Перемещаем данные из другого объекта
            provider_ = other.provider_;
            id_ = other.id_;
            // Обнуляем указатель у перемещённого объекта
            other.provider_ = nullptr;
        }
        return *this;
    }

    // Деструктор, автоматически отменяющий бронирование
    ~Booking() {
        if (provider_) {
            provider_->CancelOrComplete(*this);
        }
    }

    // Метод для получения идентификатора бронирования
    BookingId GetId() const {
        return id_;
    }

private:
    Provider* provider_;  // Указатель на провайдера
    BookingId id_;        // Идентификатор бронирования
};

}  // namespace raii