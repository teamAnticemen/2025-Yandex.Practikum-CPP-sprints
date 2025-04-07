#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <map>
#include <variant>
#include <algorithm>

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

struct RouteInfo {
    double total_time = 0.0;
    std::vector<std::variant<WaitItem, BusItem>> items;
};

class Router {
public:
    Router() = default;

    Router(const int bus_wait_time, const double bus_velocity)
        : bus_wait_time_(bus_wait_time)
        , bus_velocity_(bus_velocity) {}

    Router(const Router& settings, const transport_catalogue::TransportCatalogue& catalogue) {
        bus_wait_time_ = settings.bus_wait_time_;
        bus_velocity_ = settings.bus_velocity_;
        BuildGraph(catalogue);
    }

    const graph::DirectedWeightedGraph<double>& BuildGraph(const transport_catalogue::TransportCatalogue& catalogue);
    const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    const graph::DirectedWeightedGraph<double>& GetGraph() const;

    RouteInfo ProcessRouteRequest(const std::string_view stop_from, const std::string_view stop_to) const;

private:
    int bus_wait_time_ = 0;
    double bus_velocity_ = 0.0;

    graph::DirectedWeightedGraph<double> graph_;
    std::unordered_map<std::string, graph::VertexId> stop_ids_;
    std::unique_ptr<graph::Router<double>> router_;
};

} // namespace transport