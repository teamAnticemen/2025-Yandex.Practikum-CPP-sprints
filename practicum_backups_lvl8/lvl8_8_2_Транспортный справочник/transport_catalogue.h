#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "geo.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

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

    // Новый метод для получения автобусов по остановке
    std::vector<std::string> GetBusesByStop(const std::string& stop_name) const;

    void PrintAllBuses() const;

private:
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;
    std::unordered_map<std::string, std::vector<std::string>> stop_to_buses_map_; // Новая структура данных
};
