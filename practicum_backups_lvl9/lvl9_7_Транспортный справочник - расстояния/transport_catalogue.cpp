#include "transport_catalogue.h"
#include <chrono>
#include <iostream>

namespace transport_catalogue {

void TransportCatalogue::AddStop(const std::string_view name, const geo::Coordinates& coordinates) {
    string_storage_.emplace_back(name); 
    auto& stored_name = string_storage_.back(); 
    // Проверяем, существует ли уже запись в stops_map_
    auto it = stops_map_.find(stored_name);
    if (it != stops_map_.end()) {
        // Если запись с таким именем уже существует, обновляем соответствующий объект Stop
            const Stop* existing_stop = it->second; // Получаем указатель на уже существующий объект Stop
            const_cast<Stop*>(existing_stop)->coordinates = coordinates;
    } else {
        // Если записи не существует, добавляем новый объект Stop и обновляем карту
        stops_.emplace_back(Stop{stored_name, coordinates}); // Добавляем в дек
        stops_map_[stored_name] = &stops_.back(); // Обновляем мапу, устанавливая ссылку на только что добавленный элемент
    }
}

void TransportCatalogue::AddBus(const std::string_view name, const std::vector<const Stop*>& stops, bool is_round_trip) {
    string_storage_.emplace_back(name);
    auto& stored_name = string_storage_.back();
    buses_.emplace_back(Bus{stored_name, stops, is_round_trip});
    buses_map_[stored_name] = &buses_.back(); // Используем stored_name

    for (const auto* stop : stops) {
        stop_to_buses_map_[stop].insert(&buses_.back()); // Используем указатели
    }
}
    
void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
    if (from != nullptr && to != nullptr) {
        between_stops_distance_[{from, to}] = distance; // Добавляем расстояние в мапу
    }
    //else std::cout << "\n Error SetDistance \n";// Сюда попадать не должны
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    auto it = buses_map_.find(name);
    return it != buses_map_.end() ? it->second : nullptr;
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    auto it = stops_map_.find(name);
    return it != stops_map_.end() ? it->second : nullptr;
}

   BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
    
    const Bus* bus = FindBus(name); // Используем FindBus для поиска автобуса
    if (bus) {
        BusInfo bus_info;
        // Количество остановок
        bus_info.stop_count = bus->is_round_trip ? static_cast<int>(bus->stops.size()) : static_cast<int>(bus->stops.size() * 2 - 1);

        // Подсчет уникальных остановок без копирования
        std::unordered_map<std::string, int> stop_counter;
        for (const auto& stop : bus->stops) {
            stop_counter[stop->name]++;
        }
        bus_info.unique_stop_count = static_cast<int>(stop_counter.size());

        // Длина маршрута
        double route_length = 0.0;
        double geographical_distance = 0.0; // Географическое расстояние (D)

        if (bus->is_round_trip) {
            // Обработка кольцевого маршрута
            for (size_t i = 0; i < bus->stops.size(); ++i) {
                const auto& from_stop = bus->stops[i];
                const auto& to_stop = bus->stops[(i + 1) % bus->stops.size()]; // Кольцевой маршрут

                // Получаем расстояние от текущей остановки до следующей
                auto it = between_stops_distance_.find({from_stop, to_stop});

                if (it != between_stops_distance_.end() && it->second > 0) {
                    route_length += it->second; // Добавляем расстояние
                } else {
                    // Проверяем обратное расстояние
                    auto reverse_it = between_stops_distance_.find({to_stop, from_stop});
                    if (reverse_it != between_stops_distance_.end() && reverse_it->second > 0) {
                        route_length += reverse_it->second; // Добавляем расстояние в обратном направлении
                    }
                    //Если оба расстояния равны ноль ничего не делаем
                }

                // Добавляем географическое расстояние
                geographical_distance += geo::ComputeDistance(from_stop->coordinates, to_stop->coordinates);
            }
        } else {
            // Обработка не кольцевого маршрута
            for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
                const auto& from_stop = bus->stops[i];
                const auto& to_stop = bus->stops[i + 1];

                // Получаем расстояние от текущей остановки до следующей
                auto it = between_stops_distance_.find({from_stop, to_stop});
                auto reverse_it = between_stops_distance_.find({to_stop, from_stop});

                if (it != between_stops_distance_.end() && it->second > 0) {
                    route_length += it->second; // Добавляем расстояние
                } else {
                    // Проверяем обратное расстояние                    
                    if (reverse_it != between_stops_distance_.end() && reverse_it->second > 0) {
                        route_length += reverse_it->second; // Добавляем расстояние в обратном направлении
                        route_length += reverse_it->second; // Если расстояние туда равно ноль
                        geographical_distance += geo::ComputeDistance(from_stop->coordinates, to_stop->coordinates);
                        continue;
                    }
                    //Если оба расстояния равны ноль ничего не делаем
                }

                    if (reverse_it != between_stops_distance_.end() && reverse_it->second > 0) {
                        route_length += reverse_it->second; // Добавляем расстояние в обратном направлении

                    }
                    else //если расстояние туда не найдено
                    {
                        route_length += it->second;
                    }
                // Добавляем географическое расстояние
                geographical_distance += geo::ComputeDistance(from_stop->coordinates, to_stop->coordinates);
            }
            geographical_distance *= 2; // Учитываем обратный путь для не кольцевого маршрута
        }

        // Устанавливаем общую длину маршрута
        bus_info.route_length = route_length;

        // Вычисляем коэффициент извилистости (C)
        if (geographical_distance > 0) {
            bus_info.curvature = route_length / geographical_distance; // C = L / D
        } else {
            bus_info.curvature = 1.0; // Если географическое расстояние равно 0, устанавливаем извилистость в 1
        }            
        return bus_info; // Возвращаем уникальный указатель
    }
    return BusInfo{}; // Если автобус не найден
}


const std::unordered_set<const Bus*, CustomHash>& TransportCatalogue::GetBusesByStop(std::string_view stop_name) const {
    auto stop_it = stops_map_.find(stop_name); // Используем stop_name
    if (stop_it != stops_map_.end()) {
        const Stop* stop = stop_it->second;
        auto bus_it = stop_to_buses_map_.find(stop);
        if (bus_it != stop_to_buses_map_.end()) {
            return bus_it->second;
        }
    }
    static const std::unordered_set<const Bus*, CustomHash> empty_set;
    return empty_set;
}

/*std::unordered_map<std::string, Bus> TransportCatalogue::GetAllBuses() const {
    std::unordered_map<std::string, Bus> all_buses;
    for (const auto& bus : buses_) {
        all_buses[bus.name] = bus; // Копируем объект Bus
    }
    return all_buses; // Возвращаем копию всех автобусов
}*/

} // namespace transport_catalogue