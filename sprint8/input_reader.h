#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

namespace transport_catalogue {

    struct CommandDescription {
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;      // Название команды
        std::string id;           // id маршрута или остановки
        std::string description;  // Параметры команды
    };

    class InputReader {
    public:
        /**
         * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
         */
        void ParseLine(std::string_view line);

        /**
         * Наполняет данными транспортный справочник, используя команды из commands_
         */
        void ApplyCommands(TransportCatalogue& catalogue) const;

    private:
        /**
         * Обрабатывает команды остановок и добавляет их в каталог
         */
        void ProcessStops(TransportCatalogue& catalogue, std::vector<std::tuple<std::string, std::string, int>>& distances) const;

        /**
         * Обрабатывает команды автобусов и добавляет их в каталог
         */
        void ProcessBuses(TransportCatalogue& catalogue) const;

        /**
         * Добавляет расстояния между остановками в каталог
         */
        void ProcessDistances(TransportCatalogue& catalogue, const std::vector<std::tuple<std::string, std::string, int>>& distances) const;

        std::vector<CommandDescription> commands_;
    };
}
