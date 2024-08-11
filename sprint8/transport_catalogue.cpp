#include "transport_catalogue.h"
#include <unordered_set>
#include <algorithm>

// Добавление новой остановки
void TransportCatalogue::AddStop(const std::string& name, Coordinates coordinates) {
    Stop stop;
    stop.name = name;
    stop.coordinates = coordinates;
    stops_[name] = stop;
    stop_name_to_stop_[name] = &stops_[name];
}

// Добавление нового маршрута
void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stop_names, bool is_circular) {
    std::vector<const Stop*> stops;
    stops.reserve(stop_names.size());

    for (const auto& stop_name : stop_names) {
        if (const Stop* stop = FindStop(stop_name)) {
            stops.push_back(stop);
        } else {
            // Если остановка не найдена, добавляем её с некорректными координатами
            AddStop(stop_name, Coordinates{0.0, 0.0});
            stops.push_back(FindStop(stop_name));
        }

        // Заполняем обратный маппинг от остановок к маршрутам
        stop_to_buses_[stop_name].insert(name);
    }

    Bus& bus = buses_[name];
    bus.name = name;
    bus.stops = std::move(stops);
    bus.is_circular = is_circular;
    bus_name_to_bus_[name] = &bus;
}

// Поиск остановки по имени
const Stop* TransportCatalogue::FindStop(const std::string& name) const {
    if (auto it = stop_name_to_stop_.find(name); it != stop_name_to_stop_.end()) {
        return it->second;
    }
    return nullptr;
}

// Поиск маршрута по имени
const Bus* TransportCatalogue::FindBus(const std::string& name) const {
    if (auto it = bus_name_to_bus_.find(name); it != bus_name_to_bus_.end()) {
        return it->second;
    }
    return nullptr;
}

// Получение информации о маршруте
std::optional<BusInfo> TransportCatalogue::GetBusInfo(const std::string& bus_name) const {
    const Bus* bus = FindBus(bus_name);
    if (!bus) {
        return std::nullopt;
    }

    std::unordered_set<const Stop*> unique_stops(bus->stops.begin(), bus->stops.end());
    double total_distance = 0.0;

    if (bus->stops.size() > 1) {
        // Длина маршрута для некольцевого маршрута
        if (!bus->is_circular) {
            for (size_t i = 0; i + 1 < bus->stops.size(); ++i) {
                total_distance += ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
            }
        } else { // Длина маршрута для кольцевого маршрута
            for (size_t i = 0; i + 1 < bus->stops.size(); ++i) {
                total_distance += ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
            }
            // Возвращаемся к первой остановке для кольцевого маршрута
            total_distance += ComputeDistance(bus->stops.back()->coordinates, bus->stops.front()->coordinates);
        }
    }

    size_t stop_count = bus->is_circular ? bus->stops.size() : bus->stops.size();

    return BusInfo{
            stop_count,
            unique_stops.size(),
            total_distance
    };
}

// Получение списка автобусов, проходящих через остановку
std::optional<std::vector<std::string>> TransportCatalogue::GetBusesByStop(const std::string& stop_name) const {
    const Stop* stop = FindStop(stop_name);
    if (!stop) {
        return std::nullopt;
    }
    if (auto it = stop_to_buses_.find(stop_name); it != stop_to_buses_.end()) {
        std::vector<std::string> buses(it->second.begin(), it->second.end());
        std::sort(buses.begin(), buses.end());
        return buses;
    } else {
        return std::vector<std::string>();  // Возвращаем пустой список
    }
}