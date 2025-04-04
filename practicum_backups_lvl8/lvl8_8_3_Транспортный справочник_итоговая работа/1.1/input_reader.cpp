#include "input_reader.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <regex>

namespace input_reader {

CommandDescription InputReader::ParseCommandDescription(std::string_view line) const {
    CommandDescription command_description;
    std::string command(line);
    std::istringstream iss(command);
    
    // Получаем тип команды (Stop или Bus)
    std::getline(iss, command_description.type, ' ');

    // Получаем название остановки или маршрута
    std::string name;
    std::getline(iss, name, ':');
    command_description.name = Trim(name); // Сохраняем название с пробелами

    // Получаем параметры
    std::string params;
    std::getline(iss, params); // Получаем оставшуюся часть строки как параметры

    std::string current_param;
    if (command_description.type == "Stop") {
        // Обработка для команды Stop
        for (char c : params) {
            if (c == ',') {
                // Если текущий параметр не пустой, добавляем его в вектор
                if (!current_param.empty()) {
                    command_description.params.push_back(Trim(current_param));
                    current_param.clear(); // Очищаем текущий параметр
                }
            } else {
                // Добавляем символ к текущему параметру
                current_param += c;
            }
        }
    } else if (command_description.type == "Bus") {
        // Обработка для команды Bus
        for (char c : params) {
            if (c == '-' || c == '>') {
                // Если текущий параметр не пустой, добавляем его в вектор
                if (!current_param.empty()) {
                    command_description.params.push_back(Trim(current_param));
                    current_param.clear(); // Очищаем текущий параметр
                }
                if (c == '>') {
                    command_description.isCircular = true; // Помечаем маршрут как кольцевой
                }
            } else {
                // Добавляем символ к текущему параметру
                current_param += c;
            }
        }
    }

    // Добавляем последний параметр, если он не пустой
    if (!current_param.empty()) {
        command_description.params.push_back(Trim(current_param));
    }

    return command_description;
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    commands_.push_back(command_description);
}

void InputReader::ApplyCommands(transport_catalogue::TransportCatalogue& catalogue) const {    
    // Сначала добавляем все остановки
    for (const auto& command : commands_) {
        if (command.type == "Stop") {
            std::string name = command.name;
            double latitude = std::stod(command.params[0]);
            double longitude = std::stod(command.params[1]);
            catalogue.AddStop(name, {latitude, longitude});
        }
    }

    for (const auto& command : commands_) {
    if (command.type == "Bus") {
        std::string name = command.name;
        std::vector<std::string> stops = command.params;

        bool is_round_trip = command.isCircular;

        // Преобразуем вектор строк в вектор указателей на Stop
        std::vector<const transport_catalogue::Stop*> stop_ptrs;
        for (const auto& stop_name : stops) {
            const transport_catalogue::Stop* stop = catalogue.FindStop(stop_name);
            if (stop) {
                stop_ptrs.push_back(stop);
            }
        }

        catalogue.AddBus(name, stop_ptrs, is_round_trip);
    }
}
}

} // namespace input_reader

