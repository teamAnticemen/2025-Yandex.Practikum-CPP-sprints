#include "transport_catalogue.h"

namespace transport_catalogue {

void TransportCatalogue::AddStop(std::string_view name, const geo::Coordinates& coordinates) {
    auto stop = new Stop{name.data(), coordinates}; // Создаем Stop на куче
    stops_.push_back(stop); // Изменено: добавляем указатель на новую остановку в дек
    stops_map_[std::string(name)] = stop; // Сохраняем в мапу для поиска
}

void TransportCatalogue::AddBus(std::string_view name, const std::deque<const Stop*>& stops, bool is_round_trip) {
    auto bus = new Bus{name.data(), stops, is_round_trip}; // Создаем Bus на куче
    buses_.push_back(bus); // Изменено: добавляем указатель на новый автобус в дек
    buses_map_[std::string(name)] = bus; // Сохраняем в мапу для поиска

    // Остальная часть метода без изменений...

    // Изменено: вычисление длины маршрута
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
    // Заполняем stop_to_buses_map_
    for (const Stop* stop : stops) {
    stop_to_buses_map_[stop->name].insert(std::string(name)); // Преобразуем std::string_view в std::string
    }
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    auto it = buses_map_.find(std::string(name));
    return it != buses_map_.end() ? it->second : nullptr; // Изменено: возвращаем указатель
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    auto it = stops_map_.find(std::string(name));
    return it != stops_map_.end() ? it->second : nullptr; // Изменено: возвращаем указатель
}

BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
    auto it = bus_info_map_.find(std::string(name));
    if (it != bus_info_map_.end()) {
        return it->second; // Возвращаем информацию о маршруте
    }
    return {0, 0, 0.0}; // Возвращаем 0 для всех значений, если автобус не найден
}

std::vector<std::string> TransportCatalogue::GetBusesByStop(std::string_view stop_name) const {
    auto it = stop_to_buses_map_.find(std::string(stop_name));
    if (it != stop_to_buses_map_.end()) {
        std::unordered_set<std::string> unique_buses(it->second.begin(), it->second.end());
        return std::vector<std::string>(unique_buses.begin(), unique_buses.end());
    }
    return {};
}

std::unordered_map<std::string, Bus> TransportCatalogue::GetAllBuses() const {
    std::unordered_map<std::string, Bus> all_buses;
    for (const auto* bus : buses_) {
        all_buses[bus->name] = *bus; // Изменено: копируем информацию о каждом автобусе в новую мапу
    }
    return all_buses; // Возвращаем копию всех автобусов
}
    
TransportCatalogue::~TransportCatalogue() {
    for (const Stop* stop : stops_) {
        delete stop; // Освобождаем память для каждой остановки
    }
    for (const Bus* bus : buses_) {
        delete bus; // Освобождаем память для каждого автобуса
    }
}

} // namespace transport_catalogue
