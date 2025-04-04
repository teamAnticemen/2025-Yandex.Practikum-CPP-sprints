#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string& name, const Coordinates& coordinates) {
    stops_[name] = {name, coordinates};
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stops, bool is_round_trip) {
    buses_[name] = {name, stops, is_round_trip};

    // Заполняем соответствие остановок и автобусов
    for (const auto& stop : stops) {
        stop_to_buses_map_[stop].push_back(name);
    }
}

const Bus* TransportCatalogue::FindBus(const std::string& name) const {
    auto it = buses_.find(name);
    return it != buses_.end() ? &(it->second) : nullptr;
}

const Stop* TransportCatalogue::FindStop(const std::string& name) const {
    auto it = stops_.find(name);
    return it != stops_.end() ? &(it->second) : nullptr;
}

std::pair<int, int> TransportCatalogue::GetBusInfo(const std::string& name) const {
    const Bus* bus = FindBus(name);
    if (bus) {
        return {static_cast<int>(bus->stops.size()), static_cast<int>(std::unordered_set<std::string>(bus->stops.begin(), bus->stops.end()).size())};
    }
    return {0, 0};
}

std::vector<std::string> TransportCatalogue::GetBusesByStop(const std::string& stop_name) const {
    auto it = stop_to_buses_map_.find(stop_name);
    if (it != stop_to_buses_map_.end()) {
        // Используем unordered_set для удаления дубликатов
        std::unordered_set<std::string> unique_buses(it->second.begin(), it->second.end());
        return std::vector<std::string>(unique_buses.begin(), unique_buses.end());
    }
    return std::vector<std::string>{};
}

void TransportCatalogue::PrintAllBuses() const {
    for (const auto& [name, bus] : buses_) {
        std::cout << "Bus " << name << ": ";
        for (const auto& stop : bus.stops) {
            std::cout << stop << " ";
        }
        std::cout << "\n";
    }
}