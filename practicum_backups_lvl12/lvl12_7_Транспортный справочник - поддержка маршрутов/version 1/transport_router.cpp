#include "transport_router.h"
#include <map>
#include <algorithm>

namespace transport {

const graph::DirectedWeightedGraph<double>& Router::BuildGraph(const transport_catalogue::TransportCatalogue& catalogue) {
    const auto& all_stops = catalogue.GetSortedAllStops();
    const auto& all_buses = catalogue.GetSortedAllBuses();
    graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
    std::unordered_map<std::string, graph::VertexId> stop_ids;
    graph::VertexId vertex_id = 0;

    // Создаем вершины и ребра ожидания
    for (const auto& [stop_name, stop_info] : all_stops) {
        stop_ids[stop_info->name] = vertex_id;
        stops_graph.AddEdge(graph::Edge<double>{
            stop_info->name,  // Имя остановки
            0,                // Количество перегонов (0 для ожидания)
            vertex_id,        // Исходная вершина
            vertex_id + 1,    // Целевая вершина
            static_cast<double>(bus_wait_time_)  // Вес ребра (время ожидания)
        });
        vertex_id += 2;  // Увеличиваем vertex_id на 2 для следующей остановки
    }
    stop_ids_ = std::move(stop_ids);

    // Создаем ребра поездки на автобусе
    for (const auto& [bus_name, bus_info] : all_buses) {
    const auto& stops = bus_info->stops;
    size_t stops_count = stops.size();
    for (size_t i = 0; i < stops_count; ++i) {
        for (size_t j = i + 1; j < stops_count; ++j) {
            const transport_catalogue::Stop* stop_from = stops[i];
            const transport_catalogue::Stop* stop_to = stops[j];
            int dist_sum = 0;
            int dist_sum_inverse = 0;

            // Рассчитываем расстояние для прямого направления
            for (size_t k = i + 1; k <= j; ++k) {
                dist_sum += catalogue.GetDistance(stops[k - 1], stops[k]);
                dist_sum_inverse += catalogue.GetDistance(stops[k], stops[k - 1]);
            }

            // Преобразуем скорость из км/ч в м/с
            double speed_mps = (bus_velocity_ * 100.0) / 6.0;

                // Ребро поездки на автобусе: от stop_from к stop_to
                stops_graph.AddEdge(graph::Edge<double>{
                    bus_info->name,  // Имя автобуса
                    j - i,           // Количество перегонов
                    stop_ids_.at(stop_from->name) + 1,  // Исходная вершина
                    stop_ids_.at(stop_to->name),        // Целевая вершина
                    static_cast<double>(dist_sum) / speed_mps  // Время поездки
                });

                if (!bus_info->is_round_trip) {
                // Для некольцевых маршрутов добавляем обратное ребро
                stops_graph.AddEdge(graph::Edge<double>{
                    bus_info->name,  // Имя автобуса
                    j - i,           // Количество перегонов
                    stop_ids_.at(stop_to->name) + 1,  // Исходная вершина
                    stop_ids_.at(stop_from->name),    // Целевая вершина
                    static_cast<double>(dist_sum_inverse) / speed_mps  // Время поездки
                    });
                }
            }
        }
    }
    graph_ = std::move(stops_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);

    return graph_;
}


const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
    return router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
}

const graph::DirectedWeightedGraph<double>& Router::GetGraph() const {
    return graph_;
}

RouteInfo Router::ProcessRouteRequest(const std::string_view stop_from, const std::string_view stop_to) const {
    RouteInfo route_info;
    
    auto route = FindRoute(stop_from, stop_to);

    if (!route) {
        return route_info; // Маршрут не найден
    }

    route_info.total_time = route->weight;

    for (const auto& edge_id : route->edges) {
        const auto& edge = graph_.GetEdge(edge_id);
        if (edge.span_count == 0) {
            // Это ребро ожидания
            route_info.items.push_back(WaitItem{edge.name, edge.weight});
        } else {
            // Это ребро автобуса
            route_info.items.push_back(BusItem{edge.name, edge.span_count, edge.weight});
        }
    }

    return route_info;
}

} // namespace transport