#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

namespace transport {

struct WaitItem {
    std::string stop_name;
    double time = 0.0;
};

struct BusItem {
    std::string bus;
    size_t span_count = 0;
    double time = 0.0;
};

//Содержит только информацию о конкретном маршруте
struct RouteInfo {
    double total_time = 0.0;
    std::vector<std::variant<WaitItem, BusItem>> items;
};

//Содержит настройки работы роутера
struct RouterSettings {
    int bus_wait_time = 0;
    double bus_velocity = 0.0;
};

class Router {
public:
    Router() = default;
    
    Router(const RouterSettings& settings, const transport_catalogue::TransportCatalogue& catalogue)
        : settings_(settings) {
        BuildGraph(catalogue);
    }

    std::optional<RouteInfo> GetRouteInfo(std::string_view stop_from, std::string_view stop_to) const;

private:
    void BuildGraph(const transport_catalogue::TransportCatalogue& catalogue);
    
    RouterSettings settings_;
    graph::DirectedWeightedGraph<double> graph_;
    std::unordered_map<std::string, graph::VertexId> stop_ids_;
    std::unique_ptr<graph::Router<double>> router_;
};

} // namespace transport