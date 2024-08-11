#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>
#include <unordered_set>
#include "geo.h"

// Структура для представления остановки
struct Stop {
    std::string name;
    Coordinates coordinates;
};

// Структура для представления маршрута
struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
    bool is_circular = false;       // Является ли маршрут кольцевым
};

// Структура для хранения информации о маршруте
struct BusInfo {
    size_t stop_count;
    size_t unique_stop_count;
    double route_length;
};

// Класс транспортного каталога
class TransportCatalogue {
public:
    void AddStop(const std::string& name, Coordinates coordinates);

    void AddBus(const std::string& name, const std::vector<std::string>& stop_names, bool is_circular);

    const Stop* FindStop(const std::string& name) const;

    const Bus* FindBus(const std::string& name) const;

    std::optional<BusInfo> GetBusInfo(const std::string& bus_name) const;

    std::optional<std::vector<std::string>> GetBusesByStop(const std::string& stop_name) const;

private:
    // Контейнеры для хранения информации об остановках и маршрутах
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;

    // Карты для быстрого поиска остановок и маршрутов по их названиям
    std::unordered_map<std::string, const Stop*> stop_name_to_stop_;
    std::unordered_map<std::string, const Bus*> bus_name_to_bus_;

    // Обратная связь между остановками и маршрутами
    std::unordered_map<std::string, std::unordered_set<std::string>> stop_to_buses_;
};