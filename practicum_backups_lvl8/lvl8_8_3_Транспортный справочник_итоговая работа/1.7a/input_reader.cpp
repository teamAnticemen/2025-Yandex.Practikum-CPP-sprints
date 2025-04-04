#include "input_reader.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <regex>
#include <memory>
#include <deque>
#include <functional>

namespace input_reader {

CommandDescription InputReader::ParseCommandDescription(std::string_view line) const {
    CommandDescription command_description;
    std::string command(line);
    std::istringstream iss(command);
    
    std::getline(iss, command_description.type, ' ');
    std::string name;
    std::getline(iss, name, ':');
    command_description.name = Trim(name);

    std::string params;
    std::getline(iss, params);
    std::string current_param;
    if (command_description.type == "Stop") {
        for (char c : params) {
            if (c == ',') {
                if (!current_param.empty()) {
                    command_description.params.push_back(Trim(current_param));
                    current_param.clear();
                }
            } else {
                current_param += c;
            }
        }
    } else if (command_description.type == "Bus") {
        for (char c : params) {
            if (c == '-' || c == '>') {
                if (!current_param.empty()) {
                    command_description.params.push_back(Trim(current_param));
                    current_param.clear();
                }
                if (c == '>') {
                    command_description.isCircular = true;
                }
            } else {
                current_param += c;
            }
        }
    }
    if (!current_param.empty()) {
        command_description.params.push_back(Trim(current_param));
    }

    return command_description;
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    
    // Проверка типа команды и добавление в соответствующее место
    if (command_description.type == "Stop") {
        commands_.push_front(command_description); // Добавление команды типа Stop в начало
    } else if (command_description.type == "Bus") {
        commands_.push_back(command_description); // Добавление команды типа Bus в конец
    }
}
    
    //Разбираем команды на добавление остановок и автобуса к которому будет добавлен маршрут
    void InputReader::ApplyCommands(transport_catalogue::TransportCatalogue& catalogue) const {
    for (const auto& command : commands_) { // Итерация по std::deque
        if (command.type == "Stop") {
            std::string name = command.name;
            double latitude = std::stod(command.params[0]);
            double longitude = std::stod(command.params[1]);
            catalogue.AddStop(name, {latitude, longitude});
        }        
        
        //Это и есть место где добавляется маршрут к автобусу
        else if (command.type == "Bus") {
            std::string name = command.name;
            std::vector<std::string> stops = command.params;
            bool is_round_trip = command.isCircular;

            std::deque<std::reference_wrapper<const transport_catalogue::Stop>> stop_deque;
            for (const auto& stop_name : stops) {
                auto stop = catalogue.FindStop(stop_name);
                if (stop) {
                    stop_deque.emplace_back(*stop); // Используем reference_wrapper
                }
            }

            // Преобразование deque в vector
            std::vector<const transport_catalogue::Stop*> stop_vector;
            stop_vector.reserve(stop_deque.size()); // Резервируем место для оптимизации

            for (const auto& stop_ref : stop_deque) {
                stop_vector.push_back(&stop_ref.get()); // Добавляем указатель на Stop
            }

            // Теперь передаем stop_vector в AddBus
            catalogue.AddBus(name, stop_vector, is_round_trip);
        }
    }
}

} // namespace input_reader