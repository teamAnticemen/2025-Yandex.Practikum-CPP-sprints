#pragma once

#include <string>
#include "transport_catalogue.h"

class StatReader {
public:
    explicit StatReader(const TransportCatalogue& catalogue);
    void ProcessQuery(const std::string& bus_name) const;

private:
    const TransportCatalogue& catalogue_;
};
