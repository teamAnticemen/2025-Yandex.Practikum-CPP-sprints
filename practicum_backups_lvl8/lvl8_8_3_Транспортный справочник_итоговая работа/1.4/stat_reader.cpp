#include "stat_reader.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>

namespace stat_reader {

StatReader::StatReader(const transport_catalogue::TransportCatalogue& catalogue) : catalogue_(catalogue) {}

void StatReader::ProcessQuery(const std::string& name) const {
    if (name.substr(0, 4) == "Bus ") {
        std::string bus_name = name.substr(4);
        const transport_catalogue::Bus* bus = catalogue_.FindBus(bus_name);

        if (!bus) {
            std::cout << name << ": not found\n"; // Защита от ошибок
            return;
        }

        transport_catalogue::BusInfo bus_info = catalogue_.GetBusInfo(bus_name); // Получаем BusInfo
        
        // Получаем список остановок для данного маршрута
        std::vector<std::string> stops; // Используем вектор для хранения остановок
        catalogue_.GetStopsForBus(bus_name, stops); // Получаем остановки для маршрута
        int R = stops.size(); // Количество остановок
        int U = std::unordered_set<std::string>(stops.begin(), stops.end()).size(); // Уникальные остановки
        double length = bus_info.route_length; // Длина маршрута из BusInfo

        // Если маршрут не кольцевой
        if (!bus->is_round_trip) {
            R = (R * 2) - 1; // Общее количество остановок не кольцевого маршрута, добавляем минус один
        }

        std::cout << "Bus " << bus_name << ": " << R << " stops on route, " << U << " unique stops, ";

        // Устанавливаем точность в зависимости от длины маршрута
        if (length < 10000) {
            std::cout << std::fixed << std::setprecision(2) << length;
        } else if (length < 100000) {
            std::cout << std::fixed << std::setprecision(1) << length;
        } else if (length < 1000000) {
            std::cout << std::fixed << std::setprecision(0) << length; 
        } else {
            std::cout << std::scientific << std::setprecision(5) << length; 
        }
        std::cout << " route length\n";

    } else if (name.substr(0, 5) == "Stop ") {
        std::string stop_name = name.substr(5);
        
        const transport_catalogue::Stop* stop = catalogue_.FindStop(stop_name);

        if (!stop) {
            std::cout << "Stop " << stop_name << ": not found\n"; // Защита от ошибок
            return;
        }

        std::vector<std::string> buses; // Используем вектор для хранения автобусов
        catalogue_.GetBusesByStop(stop_name, buses); // Получаем автобусы для остановки

        if (buses.empty()) {
            std::cout << "Stop " << stop_name << ": no buses\n";
        } else {
            std::sort(buses.begin(), buses.end()); // Сортируем маршруты
            std::cout << "Stop " << stop_name << ": buses ";
            for (size_t i = 0; i < buses.size(); ++i) {
                std::cout << buses[i];
                if (i < buses.size() - 1) {
                    std::cout << " ";
                }
            }
            std::cout << "\n";
        }
    } else {
        std::cout << name << ": not a valid query\n";
    }
}

void StatReader::PrintAllBuses() const {
    for (const auto& [name, bus] : catalogue_.GetAllBuses()) {
        std::cout << "Bus " << name << ": ";
        for (const auto& stop : bus.stops) {
            std::cout << stop.get().name << " "; // Используем get() для получения ссылки на Stop
        }
        std::cout << "\n";
    }
}

} // namespace stat_reader
