#pragma once

#include "transport_catalogue.h"
#include "json.h"
#include <string>
#include <vector>

namespace json_reader {

class JsonReader {
public:
    JsonReader(transport_catalogue::TransportCatalogue& catalogue);

    // Загрузка данных из JSON
    void LoadData(const json::Node& data);

    // Обработка запросов и формирование ответа
    json::Node ProcessRequests(const json::Node& requests);

private:
    transport_catalogue::TransportCatalogue& catalogue_;
};

} // namespace json_reader