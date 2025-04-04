#include "transport_catalogue.h"

namespace transport_catalogue {

void TransportCatalogue::AddStop(const std::string_view name, const geo::Coordinates& coordinates) {
    string_storage_.emplace_back(name); 
    auto& stored_name = string_storage_.back(); // Сохраняем ссылку на строку в переменной
    stops_.emplace_back(Stop{stored_name, coordinates}); // Создаем объект Stop и добавляем его в дек
    stops_map_[stored_name] = &stops_.back(); // Сохраняем указатель на последний добавленный объект Stop
}

void TransportCatalogue::AddBus(const std::string_view name, const std::vector<const Stop*>& stops, bool is_round_trip) {
    string_storage_.emplace_back(name);
    auto& stored_name = string_storage_.back(); // Сохраняем ссылку на строку в переменной
    buses_.emplace_back(Bus{stored_name, stops, is_round_trip}); // Создаем объект Bus и добавляем его в вектор
    buses_map_[stored_name] = &buses_.back(); // Сохраняем указатель на последний добавленный объект Bus

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

    // Обновление информации о маршруте
    std::unordered_set<std::string> unique_stops;
    for (const auto& stop : stops) {
        unique_stops.insert(stop->name); // Предполагается, что stop->name - это std::string
    }

    BusInfo bus_info = {
        static_cast<int>(stops.size()), // Количество остановок
        static_cast<int>(unique_stops.size()), // Уникальные остановки
        length // Длина маршрута
    };

    bus_info_map_[stored_name] = bus_info;

    // Заполнение карты остановок
    for (const auto& stop : stops) {
        stop_to_buses_map_[stop->name].insert(stored_name); // Добавляем автобус в карту остановок
    }
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    auto it = buses_map_.find(name);
    return it != buses_map_.end() ? it->second : nullptr; // Возвращаем указатель на Bus
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {    
    auto it = stops_map_.find(name);
    return it != stops_map_.end() ? it->second : nullptr; // Возвращаем указатель на Stop
}

BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
    auto it = bus_info_map_.find(name);
    if (it != bus_info_map_.end()) {
        return it->second; // Возвращаем информацию о маршруте
    }
    return {0, 0, 0.0}; // Возвращаем 0 для всех значений, если автобус не найден
}

void TransportCatalogue::GetBusesByStop(std::string_view stop_name, std::vector<std::string>& buses) const {
    auto it = stop_to_buses_map_.find(stop_name);
    if (it != stop_to_buses_map_.end()) {
        buses.assign(it->second.begin(), it->second.end()); // Заполняем переданный вектор buses
    }
    // Если остановка не найдена, вектор buses останется пустым
}

void TransportCatalogue::GetStopsForBus(std::string_view bus_name, std::vector<std::string>& stops) const {
    auto it = buses_map_.find(bus_name);
    if (it != buses_map_.end()) {
        const Bus& bus = *it->second; // Получаем объект Bus
        for (const auto& stop_ref : bus.stops) {
            stops.push_back(stop_ref->name);
        }
    }
    // Если автобус не найден, вектор stops останется пустым
}

std::unordered_map<std::string, Bus> TransportCatalogue::GetAllBuses() const {
    std::unordered_map<std::string, Bus> all_buses;
    for (const auto& bus : buses_) {
        all_buses[bus.name] = bus; // Копируем объект Bus
    }
    return all_buses; // Возвращаем копию всех автобусов
}

} // namespace transport_catalogue