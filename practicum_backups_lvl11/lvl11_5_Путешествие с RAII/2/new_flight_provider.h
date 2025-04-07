#pragma once
#include "booking.h"  // Подключаем файл с определением шаблонного класса Booking
#include <stdexcept>
#include <string>

class FlightProvider {
public:
    using BookingId = int;
    using Booking = raii::Booking<FlightProvider>;  // Используем пространство имен raii
    friend Booking;  // Явно разрешаем функциям класса Booking вызывать private-функции нашего класса FlightProvider

    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date;
    };

    Booking Book(const BookingData&) {
        using namespace std;
        if (counter >= capacity) {
            throw runtime_error("Flight overbooking"s);
        }
        counter++;
        return {this, counter};
    }

private:
    // Скрываем эту функцию в private, чтобы её мог позвать только соответствующий friend-класс Booking
    void CancelOrComplete(const Booking&) {
        counter--;
        if (0 > counter) {
            counter = 0;
        }
    }

public:
    static int capacity;
    static int counter;
};