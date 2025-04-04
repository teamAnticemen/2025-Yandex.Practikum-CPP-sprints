#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include "geo.h"

namespace transport_catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops; // Изменено на указатели на объекты Stop
    bool is_round_trip;
};

struct BusInfo {
    int stop_count;            // Количество остановок
    int unique_stop_count;     // Количество уникальных остановок
    double route_length;       // Дистанция маршрута
};

class TransportCatalogue {
public:
    void AddStop(std::string_view name, const geo::Coordinates& coordinates);
    void AddBus(std::string_view name, const std::vector<const Stop*>& stops, bool is_round_trip);
    const Bus* FindBus(std::string_view name) const;
    const Stop* FindStop(std::string_view name) const;

    // Методы для получения информации о маршруте
    BusInfo GetBusInfo(std::string_view name) const;
    // Новый метод для получения автобусов по остановке
    std::vector<std::string> GetBusesByStop(std::string_view stop_name) const;
    // Печать всех автобусов с их маршрутами
    std::unordered_map<std::string, Bus> GetAllBuses() const;

    private:
    std::unordered_map<std::string, BusInfo> bus_info_map_;
    std::unordered_map<std::string, Stop> stops_; // Хранит остановки
    std::unordered_map<std::string, Bus> buses_; // Хранит автобусы
    std::unordered_map<std::string, std::vector<std::string>> stop_to_buses_map_; // Хранит соответствие остановок и автобусов
};

} // namespace transport_catalogue

