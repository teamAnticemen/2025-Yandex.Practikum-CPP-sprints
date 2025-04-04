#pragma once

#include <string_view>
#include <vector>
#include <string>
#include "transport_catalogue.h"

struct CommandDescription {
    std::string type;
    std::string name;
    std::vector<std::string> params;
    bool isCircular = false; // Поле для указания кольцевого маршрута
};

class InputReader {
public:
    void ParseLine(std::string_view line);
    void ApplyCommands(TransportCatalogue& catalogue) const;

private:
    CommandDescription ParseCommandDescription(std::string_view line) const;
    std::vector<CommandDescription> commands_; // Хранит команды
};