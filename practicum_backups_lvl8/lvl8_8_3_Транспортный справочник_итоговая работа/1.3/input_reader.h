#pragma once

#include <deque>
#include <string_view>
#include <vector>
#include <string>
#include "transport_catalogue.h"

namespace input_reader {

struct CommandDescription {
    std::string type; //Bus или Stop
    std::string name;
    std::vector<std::string> params; //Перечень параметров
    bool isCircular = false; // Поле для указания кольцевого маршрута
};

class InputReader {
public:
    void ParseLine(std::string_view line);
    void ApplyCommands(transport_catalogue::TransportCatalogue& catalogue) const;

private:
    CommandDescription ParseCommandDescription(std::string_view line) const;
    std::vector<CommandDescription> commands_; // Хранит команды
    std::string Trim(const std::string& str) const {
        size_t first = str.find_first_not_of(" \t");
        if (first == std::string::npos) return ""; // пустая строка
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, last - first + 1);
    }
};

} // namespace input_reader
