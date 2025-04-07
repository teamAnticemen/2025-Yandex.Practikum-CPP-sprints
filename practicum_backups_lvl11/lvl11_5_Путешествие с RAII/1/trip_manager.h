#pragma once
#include "flight_provider.h"
#include "hotel_provider.h"

#include <string>
#include <vector>

class Trip {
public:
    Trip(HotelProvider& hp, FlightProvider& fp)
        : hotel_provider_(hp), flight_provider_(fp) {}

    Trip(const Trip& other)
        : hotel_provider_(other.hotel_provider_), flight_provider_(other.flight_provider_),
          flights(other.flights), hotels(other.hotels) {}

    Trip(Trip&& other) noexcept
        : hotel_provider_(other.hotel_provider_), flight_provider_(other.flight_provider_),
          flights(std::move(other.flights)), hotels(std::move(other.hotels)) {}

    Trip& operator=(const Trip& other) {
        if (this != &other) {
            hotel_provider_ = other.hotel_provider_;
            flight_provider_ = other.flight_provider_;
            flights = other.flights;
            hotels = other.hotels;
        }
        return *this;
    }

    Trip& operator=(Trip&& other) noexcept {
        if (this != &other) {
            hotel_provider_ = other.hotel_provider_;
            flight_provider_ = other.flight_provider_;
            flights = std::move(other.flights);
            hotels = std::move(other.hotels);
        }
        return *this;
    }

    void Cancel() {
        for (auto& flight : flights) {
            flight_provider_.Cancel(flight);
        }
        for (auto& hotel : hotels) {
            hotel_provider_.Cancel(hotel);
        }
        flights.clear();
        hotels.clear();
    }

    ~Trip() {
        Cancel();
    }

    void AddFlight(FlightProvider::BookingId flight) {
        flights.push_back(flight);
    }

    void AddHotel(HotelProvider::BookingId hotel) {
        hotels.push_back(hotel);
    }

private:
    HotelProvider& hotel_provider_;
    FlightProvider& flight_provider_;
    std::vector<FlightProvider::BookingId> flights;
    std::vector<HotelProvider::BookingId> hotels;
};

class TripManager {
public:
    using BookingId = std::string;
    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date_from;
        std::string date_to;
    };

    Trip Book(const BookingData& data) {
        (void)data;  // Помечаем параметр как неиспользуемый
        Trip trip(hotel_provider_, flight_provider_);
        {
            FlightProvider::BookingData flight_booking_data;
            trip.AddFlight(flight_provider_.Book(flight_booking_data));
        }
        {
            HotelProvider::BookingData hotel_booking_data;
            trip.AddHotel(hotel_provider_.Book(hotel_booking_data));
        }
        {
            FlightProvider::BookingData flight_booking_data;
            trip.AddFlight(flight_provider_.Book(flight_booking_data));
        }
        return trip;
    }

    void Cancel(Trip& trip) {
        trip.Cancel();
    }

private:
    HotelProvider hotel_provider_;
    FlightProvider flight_provider_;
};