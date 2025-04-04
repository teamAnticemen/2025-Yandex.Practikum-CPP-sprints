#include "transport_catalogue.h"

namespace transport_catalogue {

void TransportCatalogue::AddStop(std::string_view name, const geo::Coordinates& coordinates) {
    stops_[std::string(name)] = {name.data(), coordinates}; // Конвертируем std::string_view в std::string
}

void TransportCatalogue::AddBus(std::string_view name, const std::vector<const Stop*>& stops, bool is_round_trip) {
    // Сохраняем автобус
    buses_[std::string(name)] = {name.data(), stops, is_round_trip};

    // Добавляем автобус в список автобусов для каждой остановки
    for (const auto* stop : stops) {
        if (stop) { // Проверяем, что указатель не нулевой
            stop_to_buses_map_[stop->name].push_back(name.data());
        }
    }

    // Вычисление длины маршрута
    double length = 0.0;
    if (is_round_trip) {
        for (size_t i = 0; i < stops.size(); ++i) {
            const Stop* from = stops[i];
            const Stop* to = stops[(i + 1) % stops.size()];
            length += geo::ComputeDistance(from->coordinates, to->coordinates);
        }
    } else {
        for (size_t i = 0; i < stops.size() - 1; ++i) {
            const Stop* from = stops[i];
            const Stop* to = stops[i + 1];
            length += geo::ComputeDistance(from->coordinates, to->coordinates);
        }
        length *= 2; // Учитываем обратный путь
    }

    // Обновляем информацию о маршруте
    BusInfo bus_info = {
        static_cast<int>(stops.size()), // Количество остановок
        static_cast<int>(std::unordered_set<const Stop*>(stops.begin(), stops.end()).size()), // Уникальные остановки
        length // Длина маршрута
    };
    
    bus_info_map_[std::string(name)] = bus_info; // Сохраняем BusInfo в карту
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    auto it = buses_.find(std::string(name)); // Конвертируем std::string_view в std::string
    return it != buses_.end() ? &(it->second) : nullptr;
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    auto it = stops_.find(std::string(name)); // Конвертируем std::string_view в std::string
    return it != stops_.end() ? &(it->second) : nullptr;
}

BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
    auto it = bus_info_map_.find(std::string(name));
    if (it != bus_info_map_.end()) {
        return it->second; // Возвращаем информацию о маршруте
    }
    return {0, 0, 0.0}; // Возвращаем 0 для всех значений, если автобус не найден
}

std::vector<std::string> TransportCatalogue::GetBusesByStop(std::string_view stop_name) const {
    auto it = stop_to_buses_map_.find(std::string(stop_name)); // Конвертируем std::string_view в std::string
    if (it != stop_to_buses_map_.end()) {
        // Используем unordered_set для удаления дубликатов
        std::unordered_set<std::string> unique_buses(it->second.begin(), it->second.end());
        return std::vector<std::string>(unique_buses.begin(), unique_buses.end());
    }
    return {};
}

std::unordered_map<std::string, Bus> TransportCatalogue::GetAllBuses() const {
    return buses_; // Возвращаем копию всех автобусов
}

} // namespace transport_catalogue
