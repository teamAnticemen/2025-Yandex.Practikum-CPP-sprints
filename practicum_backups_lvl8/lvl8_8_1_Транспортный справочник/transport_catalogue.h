#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "geo.h"
#include <iostream>
#include <sstream>  // Для std::istringstream
#include <algorithm> // Для std::remove_if
#include <cctype> // Для std::isspace

struct Stop {
    std::string name;
    Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<std::string> stops;
    bool is_round_trip;
};

class TransportCatalogue {
public:
    void AddStop(const std::string& name, const Coordinates& coordinates);
    void AddBus(const std::string& name, const std::vector<std::string>& stops, bool is_round_trip);
    const Bus* FindBus(const std::string& name) const;
    const Stop* FindStop(const std::string& name) const;
    
    // Методы для получения информации о маршруте
    std::pair<int, int> GetBusInfo(const std::string& name) const;
    
    void PrintAllBuses() const {
        if (buses_.empty()) {
            std::cout << "No buses in the catalogue." << std::endl;
            return;
        }

        for (const auto& [bus_name, bus] : buses_) {
            std::cout << "Bus: " << bus_name << "\nStops: ";
            for (const auto& stop : bus.stops) {
                std::cout << stop << ", ";
            }
            std::cout << std::endl;
        }
    }

private:
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;
};