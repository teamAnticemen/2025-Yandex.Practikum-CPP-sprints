#include "input_reader.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <regex>

CommandDescription InputReader::ParseCommandDescription(std::string_view line) const {
    CommandDescription command_description;
    std::string command(line);
    std::istringstream iss(command);
    
    // Получаем тип команды (Stop или Bus)
    std::getline(iss, command_description.type, ' ');

    // Получаем название остановки или маршрута
    std::string name;
    std::getline(iss, name, ':');
    command_description.name = name; // Сохраняем название с пробелами

    // Получаем параметры
    std::string params;
    std::getline(iss, params); // Получаем оставшуюся часть строки как параметры

    std::string current_param;
    for (char c : params) {
        // Проверяем, является ли текущий символ разделителем
        if (c == ',' || c == '-') {
            // Если текущий параметр не пустой, добавляем его в вектор
            if (!current_param.empty()) {
                // Удаляем пробелы в начале и в конце
                current_param.erase(0, current_param.find_first_not_of(" \t"));
                current_param.erase(current_param.find_last_not_of(" \t") + 1);

                // Добавляем текущий параметр, если он не пустой
                command_description.params.push_back(current_param);
                current_param.clear(); // Очищаем текущий параметр
            }
        } else if (c == '>') {
            // Если встречаем разделитель '>', помечаем маршрут как кольцевой
            command_description.isCircular = true;

            // Добавляем текущий параметр, если он не пустой
            if (!current_param.empty()) {
                current_param.erase(0, current_param.find_first_not_of(" \t"));
                current_param.erase(current_param.find_last_not_of(" \t") + 1);

                // Добавляем текущий параметр, если он не пустой
                command_description.params.push_back(current_param);
                current_param.clear(); // Очищаем текущий параметр
            }
            // После добавления текущего параметра продолжаем обработку
            continue; // Продолжаем цикл, чтобы обработать оставшиеся остановки
        } else {
            // Добавляем символ к текущему параметру
            current_param += c;
        }
    }
    
    // Добавляем последний параметр, если он не пустой
    if (!current_param.empty()) {
        current_param.erase(0, current_param.find_first_not_of(" \t"));
        current_param.erase(current_param.find_last_not_of(" \t") + 1);

        // Добавляем текущий параметр, если он не пустой
        command_description.params.push_back(current_param);
    }

    return command_description;
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    commands_.push_back(command_description);
}

void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {    
    for (const auto& command : commands_) {
        if (command.type == "Stop") {
            std::string name = command.name;
            double latitude = std::stod(command.params[0]);
            double longitude = std::stod(command.params[1]);
            catalogue.AddStop(name, {latitude, longitude});
        } else if (command.type == "Bus") {
            std::string name = command.name;
            std::vector<std::string> stops = command.params; // Здесь предполагается, что все параметры - это остановки

            // Проверяем статус кольцевого маршрута
            bool is_round_trip = command.isCircular; // Используем значение isCircular из command

            // Если маршрут кольцевой, не нужно удалять последний параметр, так как он уже помечен
            catalogue.AddBus(name, stops, is_round_trip);
        }
    }
}