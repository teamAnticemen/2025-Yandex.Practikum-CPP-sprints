#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string& name, const Coordinates& coordinates) {
    stops_.emplace(name, Stop{name, coordinates});
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stops, bool is_round_trip) {
    buses_.emplace(name, Bus{name, stops, is_round_trip});
}

const Bus* TransportCatalogue::FindBus(const std::string& name) const {
    auto it = buses_.find(name);
    return it != buses_.end() ? &it->second : nullptr;
}

const Stop* TransportCatalogue::FindStop(const std::string& name) const {
    auto it = stops_.find(name);
    return it != stops_.end() ? &it->second : nullptr;
}

std::pair<int, int> TransportCatalogue::GetBusInfo(const std::string& name) const {
    const Bus* bus = FindBus(name);
    if (!bus) {
        return {0, 0}; // Маршрут не найден
    }
    // Общее количество остановок
    int total_stops = static_cast<int>(bus->stops.size());
    //std::cout << "bus name -> " << name << " stop size -> " << total_stops << std::endl;
    // Для получения уникальных остановок используем unordered_set
    std::unordered_set<std::string> unique_stops(bus->stops.begin(), bus->stops.end());
    int unique_count = static_cast<int>(unique_stops.size());

    return {total_stops, unique_count}; // Возвращаем пару (общее количество остановок, количество уникальных остановок)
}

