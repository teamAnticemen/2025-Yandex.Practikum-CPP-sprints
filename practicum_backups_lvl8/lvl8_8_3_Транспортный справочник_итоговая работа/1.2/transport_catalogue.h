#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>
#include "geo.h" 

namespace transport_catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::deque<const Stop*> stops; // Изменено: используется deque для хранения указателей на остановки
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
    void AddBus(std::string_view name, const std::deque<const Stop*>& stops, bool is_round_trip);
    const Bus* FindBus(std::string_view name) const;
    const Stop* FindStop(std::string_view name) const;
    BusInfo GetBusInfo(std::string_view name) const;
    std::vector<std::string> GetBusesByStop(std::string_view stop_name) const;
    std::unordered_map<std::string, Bus> GetAllBuses() const;
    ~TransportCatalogue(); // Деструктор для освобождения памяти

private:
    std::vector<Stop*> stops_; // Вектор указателей на остановки
    std::vector<Bus*> buses_; // Вектор указателей на автобусы
    std::unordered_map<std::string, Stop*> stops_map_; // Мапа для быстрого поиска остановок
    std::unordered_map<std::string, Bus*> buses_map_; // Мапа для быстрого поиска автобусов
    std::unordered_map<std::string, BusInfo> bus_info_map_; // Мапа для хранения информации о маршрутах
    std::unordered_map<std::string, std::unordered_set<std::string>> stop_to_buses_map_; // Мапа для хранения автобусов по остановкам
};

} // namespace transport_catalogue
