#include "transport_catalogue.h"
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <vector>
#include <stdexcept>
#include "geo.h"
namespace transport_catalogue {

// Добавление новой остановки
    void TransportCatalogue::AddStop(const std::string& name, Coordinates coordinates) {
        // Создаем объект Stop и добавляем его в контейнер stops_
        stops_[name] = {name, coordinates};
        stop_names_.push_back(name);
    }

// Добавление нового маршрута
    void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stop_names, bool is_circular) {
        Bus new_bus;
        new_bus.name = name;
        new_bus.is_circular = is_circular;

        for (const auto& stop_name : stop_names) {
            const Stop* stop_ptr = FindStop(stop_name);
            if (stop_ptr != nullptr) {
                new_bus.stops.push_back(stop_ptr);
                buses_by_stop_[stop_ptr->name].insert(name);
            }
        }

        buses_[name] = std::move(new_bus);
    }



// Поиск маршрута по имени
    const Stop* TransportCatalogue::FindStop(const std::string_view& name) const {
        auto it = stops_.find(std::string(name));
        if (it == stops_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    void TransportCatalogue::SetDistance(const std::string &from, const std::string &to, int distance) {
        const Stop* from_stop = FindStop(from);
        const Stop* to_stop = FindStop(to);
        if (from_stop && to_stop) {
            distances_[{from_stop, to_stop}] = distance;
        }

    }

    const std::vector<const Stop*>& TransportCatalogue::GetBusStops(const std::string& bus_name) const {
        auto bus_it = buses_.find(bus_name);
        if (bus_it == buses_.end()) {
            throw std::out_of_range("Bus not found");
        }
        return bus_it->second.stops;
    }

// Получение информации о маршруте
    std::tuple<int, int, double, double> TransportCatalogue::GetBusInfo(const std::string& bus_name) const {
        auto bus_it = buses_.find(bus_name);
        if (bus_it == buses_.end()) {
            throw std::out_of_range("Bus not found");
        }

        const auto& bus = bus_it->second;
        std::unordered_set<const Stop*> unique_stops(bus.stops.begin(), bus.stops.end());
        double total_length = 0;
        double geo_length = 0;

        for (size_t i = 0; i + 1 < bus.stops.size(); ++i) {
            const Stop* from = bus.stops[i];
            const Stop* to = bus.stops[i + 1];
            try {
                total_length += GetDistance(from->name, to->name);
            }
            catch (const std::out_of_range&) {

            }
            geo_length += ComputeDistance(from->coordinates, to->coordinates);
        }

        double curvature = geo_length > 0 ? total_length / geo_length : 0;

        return { static_cast<int>(bus.stops.size()), static_cast<int>(unique_stops.size()),
                 total_length, curvature };
    }


// Получение списка автобусов, проходящих через остановку
    std::vector<std::string> TransportCatalogue::GetBusesByStop(const std::string& stop_name) const {
        auto stop_it = stops_.find(stop_name);
        if (stop_it == stops_.end()) {
            throw std::out_of_range("Stop not found");
        }

        auto buses_it = buses_by_stop_.find(stop_name);
        if (buses_it == buses_by_stop_.end()) {
            return {};
        }

        std::vector<std::string> buses(buses_it->second.begin(), buses_it->second.end());
        std::sort(buses.begin(), buses.end());
        return buses;
    }

    const Bus* TransportCatalogue::FindBus(const std::string_view& name) const {
        auto it = buses_.find(std::string(name));
        if (it == buses_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    int TransportCatalogue::GetDistance(const std::string& from, const std::string& to) const {
        const Stop* from_stop = FindStop(from);
        const Stop* to_stop = FindStop(to);
        if (!from_stop || !to_stop) {
            throw std::out_of_range("Distance not found");
        }

        auto it = distances_.find({ from_stop, to_stop });
        if (it != distances_.end()) {
            return it->second;
        }

        it = distances_.find({ to_stop, from_stop });
        if (it != distances_.end()) {
            return it->second;
        }

        throw std::out_of_range("Distance not found");
    }
}
