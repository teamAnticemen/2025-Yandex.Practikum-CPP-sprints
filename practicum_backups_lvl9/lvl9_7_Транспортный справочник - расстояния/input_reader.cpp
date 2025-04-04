#include "input_reader.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <regex>
#include <memory>
#include <deque>
#include <functional>

using namespace std::literals;
namespace input_reader {
    std::string_view Trimm(std::string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }
    std::vector<std::string_view> MySplit(std::string_view string, char delim) {
        std::vector<std::string_view> result;

        size_t pos = 0;
        while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
            auto delim_pos = string.find(delim, pos);
            if (delim_pos == string.npos) {
                delim_pos = string.size();
            }
            if (auto substr = Trimm(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                result.push_back(substr);
            }
            pos = delim_pos + 1;
        }

        return result;
    }
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

       auto comma1 = params.find(',');
       auto comma2 = params.find(',', comma1 + 1);
       auto str = params.substr(0,comma2);
       auto not_space = str.find_first_not_of(' ');
       auto comma = str.find(',');

       auto not_space2 = str.find_first_not_of(' ', comma + 1);

       auto lat = (std::string(str.substr(not_space, comma - not_space)));
       auto lng = (std::string(str.substr(not_space2)));
       command_description.params.push_back(lat);
       command_description.params.push_back(lng);




       // Подсчет количества запятых

    // Извлекаем расстояния, если они есть
    if (comma2!= params.npos) {
        params = params.substr(comma2+1);
        auto parts = MySplit(params,',');
        static const auto m_to_text = "m to "sv;
        for(auto part:parts){
            auto m_to_pos = part.find(m_to_text);
            auto dist = std::string(part.substr(0, m_to_pos));
            auto dest = std::string(part.substr(m_to_pos + m_to_text.size()));
            command_description.params.push_back(dest);
            command_description.params.push_back(dist);
        }

    }
}

 else if (command_description.type == "Bus") {
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
        // Проверяем количество параметров
            if (command.params.size() > 2) {
                // Сначала добавляем остановку
                catalogue.AddStop(command.name, {std::stod(command.params[0]), std::stod(command.params[1])});

                // Затем добавляем расстояния между остановками
                for (size_t j = 2; j < command.params.size(); j += 2) {
                    std::string neighbor_stop = command.params[j]; // Имя соседней остановки
                    int distance = std::stoi(command.params[j + 1]); // Расстояние до соседней остановки

                    // Теперь добавляем расстояние в мапу
                    // Предполагаем, что у нас есть метод для получения указателя на остановку
                    const auto* from_stop = catalogue.FindStop(command.name);
                    const auto* to_stop = catalogue.FindStop(neighbor_stop);
                    if(to_stop == nullptr) 
                    {
                        catalogue.AddStop(neighbor_stop, {0, 0});
                        to_stop = catalogue.FindStop(neighbor_stop);
                    }
                    if (from_stop && to_stop) {
                        catalogue.SetDistance(from_stop, to_stop, distance); // Добавляем расстояние в мапу
                    }
                }
            } else {
                // Если нет дополнительных параметров, просто добавляем остановку
                catalogue.AddStop(command.name, {std::stod(command.params[0]), std::stod(command.params[1])});
            }
    }         
        
        //Это и есть место где добавляется маршрут к автобусу
        else if (command.type == "Bus") {
            bool is_round_trip = command.isCircular;

            std::deque<std::reference_wrapper<const transport_catalogue::Stop>> stop_deque;
            for (const auto& stop_name : command.params) {
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
            catalogue.AddBus(command.name, stop_vector, is_round_trip);
        }
    }
}

} // namespace input_reader