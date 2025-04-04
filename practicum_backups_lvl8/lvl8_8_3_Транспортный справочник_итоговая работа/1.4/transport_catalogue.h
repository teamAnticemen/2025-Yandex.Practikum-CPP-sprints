#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>
#include <functional> // Для std::reference_wrapper
#include "geo.h" 

namespace transport_catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::deque<std::reference_wrapper<const Stop>> stops; // Изменено: используется deque для хранения ссылок на остановки
    bool is_round_trip;
};

struct BusInfo {
    int stop_count;            // Количество остановок
    int unique_stop_count;     // Количество уникальных остановок
    double route_length;       // Дистанция маршрута
};

class TransportCatalogue {
public:
    void AddStop(const std::string_view name, const geo::Coordinates& coordinates);
    void AddBus(const std::string_view name, const std::deque<std::reference_wrapper<const Stop>>& stops, bool is_round_trip); // Изменено: принимает ссылки
    const Bus* FindBus(std::string_view name) const;
    const Stop* FindStop(std::string_view name) const;
    BusInfo GetBusInfo(std::string_view name) const;
    void GetBusesByStop(std::string_view stop_name, std::vector<std::string>& buses) const;
    void GetStopsForBus(std::string_view bus_name, std::vector<std::string>& stops) const;
    std::unordered_map<std::string, Bus> GetAllBuses() const;

private:
    std::deque<std::string> string_storage_; // Вектор для хранения строк, должно убрать непредвиденное поведение при использовании string_view
    std::deque<Stop> stops_; // Дек объектов Stop
    std::deque<Bus> buses_;   // Дек объектов Bus
    std::unordered_map<std::string_view, const Stop*> stops_map_; // Указатели на Stop
    std::unordered_map<std::string_view, const Bus*> buses_map_; // Указатели на Bus
    std::unordered_map<std::string_view, BusInfo> bus_info_map_; // Мапа для хранения информации о маршрутах
    std::unordered_map<std::string_view, std::unordered_set<std::string>> stop_to_buses_map_; // Мапа для хранения автобусов по остановкам
};

} // namespace transport_catalogue
