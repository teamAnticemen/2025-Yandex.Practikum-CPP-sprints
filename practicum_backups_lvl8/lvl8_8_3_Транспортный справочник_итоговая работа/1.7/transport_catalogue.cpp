#include "transport_catalogue.h"

namespace transport_catalogue {

void TransportCatalogue::AddStop(const std::string_view name, const geo::Coordinates& coordinates) {
    string_storage_.emplace_back(name); 
    auto& stored_name = string_storage_.back(); 
    stops_.emplace_back(Stop{stored_name, coordinates}); 
    stops_map_[stored_name] = &stops_.back();
}

void TransportCatalogue::AddBus(const std::string_view name, const std::vector<const Stop*>& stops, bool is_round_trip) {
    string_storage_.emplace_back(name); // Сохраняем имя автобуса
    auto& stored_name = string_storage_.back(); // Сохраняем ссылку на строку в переменной
    buses_.emplace_back(Bus{stored_name, stops, is_round_trip}); // Создаем объект Bus и добавляем его в дек
    buses_map_[stored_name] = &buses_.back(); // Добавляем автобус в карту

    // Обновляем stop_to_buses_map_
    for (const auto* stop : stops) {
        stop_to_buses_map_[stop->name].insert(stored_name); // Добавляем автобус к остановке
    }
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    auto it = buses_map_.find(name);
    if (it != buses_map_.end()) {
        return it->second; 
    }
    return nullptr;
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    auto it = stops_map_.find(name);
    if (it != stops_map_.end()) {
        return it->second; 
    }
    return nullptr;
}


BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
    for (const auto& bus : buses_) {
        if (bus.name == name) {
            BusInfo bus_info;

            // Количество остановок
            if(bus.is_round_trip)
            bus_info.stop_count = static_cast<int>(bus.stops.size());
            else
            {
            bus_info.stop_count = static_cast<int>((bus.stops.size()*2) - 1);    
            }

            // Уникальные остановки
            std::unordered_set<std::string> unique_stops;
            for (const auto& stop : bus.stops) {
                unique_stops.insert(stop->name); // Предполагается, что stop->name - это std::string
            }
            
            bus_info.unique_stop_count = static_cast<int>(unique_stops.size());

            // Длина маршрута
            double length = 0.0;
            if (bus.is_round_trip) {
                for (size_t i = 0; i < bus.stops.size(); ++i) {
                    const Stop* from = bus.stops[i];
                    const Stop* to = bus.stops[(i + 1) % bus.stops.size()];
                    length += geo::ComputeDistance(from->coordinates, to->coordinates);
                }
            } else {
                for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
                    const Stop* from = bus.stops[i];
                    const Stop* to = bus.stops[i + 1];
                    length += geo::ComputeDistance(from->coordinates, to->coordinates);
                }
                length *= 2; //Учитываем обратный путь
            }
            bus_info.route_length = length;

            return bus_info; // Возвращаем вычисленную информацию о маршруте
        }
    }
    return {0, 0, 0.0}; // Возвращаем структуру с нулевыми значениями, если автобус не найден
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