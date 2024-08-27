#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <unordered_set>
#include <string_view>
#include "geo.h"

namespace transport_catalogue {
// Структура для представления остановки
struct Stop {
    std::string name;
    Coordinates coordinates;
};

// Структура для представления маршрута
struct Bus {
    std::string name;
    std::vector<std::string> stops;
    bool is_circular = false;       // Является ли маршрут кольцевым
};

// Класс транспортного каталога
class TransportCatalogue {
public:
    void AddStop(const Stop& stop);

    void AddBus(const Bus& bus);

    void SetDistance(const std::string& from, const std::string& to, int distance);

    const Stop* FindStop(const std::string_view& name) const;

    const Bus* FindBus(const std::string& name) const;

    const std::vector<std::string>& GetBusStops(const std::string& bus_name) const;

    std::vector<std::string> GetBusesByStop(const std::string& stop_name) const;

    std::tuple<int, int, double, double> GetBusInfo(const std::string& bus_name) const;

    int GetDistance(const std::string& from, const std::string& to) const;

private:

    // Хеш-функция для пары указателей на остановки
    struct PairHash {
        size_t operator()(const std::pair<const Stop*, const Stop*>& pair) const {
            return std::hash<const void*>()(pair.first) ^ std::hash<const void*>()(pair.second);
        }
    };
    // Контейнеры для хранения информации об остановках и маршрутах
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;

    // Карты для быстрого поиска остановок и маршрутов по их названиям
    std::vector<std::string> stop_names_;

    std::unordered_map<std::string_view, std::unordered_set<std::string>> buses_by_stop_;

    // Хранение расстояниz
    std::unordered_map<std::pair<const Stop*, const Stop*>, double, PairHash> distances_;
};
}
