#include "stat_reader.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

StatReader::StatReader(const TransportCatalogue& catalogue) : catalogue_(catalogue) {}

void StatReader::ProcessQuery(const std::string& name) const {
        if (name.substr(0, 4) == "Bus ") {
        // Обработка запроса на автобус
        std::string bus_name = name.substr(4); // Убираем префикс
        const Bus* bus = catalogue_.FindBus(bus_name);

        if (!bus) {
            std::cout << name << ": not found\n";
            return;
        }

        auto isCircular = bus->is_round_trip;
        auto [R, U] = catalogue_.GetBusInfo(bus_name);
        
        // Если маршрут не кольцевой, удваиваем количество уникальных остановок
        if (!isCircular) {
            R = (R * 2) - 1; // Общее количество остановок будет удвоено
        }

        double length = 0; // Здесь можно добавить логику для вычисления длины маршрута

        // Вычисление длины маршрута
        if (bus->is_round_trip) {
            for (size_t i = 0; i < bus->stops.size(); ++i) {
                const Stop* from = catalogue_.FindStop(bus->stops[i]);
                const Stop* to = catalogue_.FindStop(bus->stops[(i + 1) % bus->stops.size()]);
                length += ComputeDistance(from->coordinates, to->coordinates);
            }
        } else {
            for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
                const Stop* from = catalogue_.FindStop(bus->stops[i]);
                const Stop* to = catalogue_.FindStop(bus->stops[i + 1]);
                length += ComputeDistance(from->coordinates, to->coordinates);
            }
            length *= 2; // Учитываем обратный путь
        }

        std::cout << "Bus " << bus_name << ": " << R << " stops on route, " << U << " unique stops, ";

        // Устанавливаем точность в зависимости от длины маршрута
        if (length < 10000) {
            std::cout << std::fixed << std::setprecision(2) << length;
        } else if (length < 100000) {
            std::cout << std::fixed << std::setprecision(1) << length;
        } else if (length < 1000000) {
            std::cout << std::fixed << std::setprecision(0) << length; // Если длина меньше 1,000,000
        } else {
            std::cout << std::scientific << std::setprecision(5) << length; // Если длина больше 1,000,000, выводим в научной нотации с 4 знаками после запятой
        }
        std::cout << " route length\n";

    } else if (name.substr(0, 5) == "Stop ") {
        // Обработка запроса на остановку
        std::string stop_name = name.substr(5); // Убираем префикс
        const Stop* stop = catalogue_.FindStop(stop_name);

        if (!stop) {
            std::cout << "Stop " << stop_name << ": not found\n";
            return;
        }

        auto buses = catalogue_.GetBusesByStop(stop_name);

        if (buses.empty()) {
            std::cout << "Stop " << stop_name << ": no buses\n";
        } else {
            std::sort(buses.begin(), buses.end()); // Сортируем маршруты
            std::cout << "Stop " << stop_name << ": buses ";
            for (const auto& bus : buses) {
                std::cout << bus << " ";
            }
            std::cout << "\n";
        }
    } else {
        // Если запрос не является ни автобусом, ни остановкой
        std::cout << name << ": not a valid query\n";
    }
}