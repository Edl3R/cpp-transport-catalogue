#include "input_reader.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <regex>
namespace transport_catalogue {

/**
 * Удаляет пробелы в начале и конце строки
 */
    std::string_view Trim(std::string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        const auto end = string.find_last_not_of(' ');
        return string.substr(start, end + 1 - start);
    }



/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
    Coordinates ParseCoordinates(std::string_view str) {
        static const double nan = std::nan("");

        str = Trim(str);

        auto comma = str.find(',');
        if (comma == str.npos) {
            return { nan, nan };
        }

        auto lat_str = Trim(str.substr(0, comma));
        auto lng_str = Trim(str.substr(comma + 1));

        double lat;
        double lng;
        try {
            lat = std::stod(std::string(lat_str));
            lng = std::stod(std::string(lng_str));
        }
        catch (const std::invalid_argument&) {
            return { nan, nan };
        }
        catch (const std::out_of_range&) {
            return { nan, nan };
        }

        return { lat, lng };
    }




    /**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
    std::vector<std::string_view> Split(std::string_view string, char delim) {
        std::vector<std::string_view> result;

        size_t pos = 0;
        while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
            auto delim_pos = string.find(delim, pos);
            if (delim_pos == string.npos) {
                delim_pos = string.size();
            }
            if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                result.push_back(substr);
            }
            pos = delim_pos + 1;
        }

        return result;
    }

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
    std::vector<std::string_view> ParseRoute(std::string_view route) {
        if (route.find('>') != route.npos) {
            return Split(route, '>');
        }

        auto stops = Split(route, '-');
        std::vector<std::string_view> results(stops.begin(), stops.end());
        results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

        return results;
    }

    CommandDescription ParseCommandDescription(std::string_view line) {
        auto colon_pos = line.find(':');
        if (colon_pos == line.npos) {
            return {};
        }

        auto space_pos = line.find(' ');
        if (space_pos >= colon_pos || space_pos == line.npos) {
            return {};
        }

        auto not_space = line.find_first_not_of(' ', space_pos);
        if (not_space >= colon_pos || not_space == line.npos) {
            return {};
        }

        std::string command = std::string(line.substr(0, space_pos));
        std::string id = std::string(line.substr(not_space, colon_pos - not_space));
        std::string description = std::string(line.substr(colon_pos + 1));

        description = std::string(Trim(description));

        return { command, id, description };
    }

    void InputReader::ParseLine(std::string_view line) {
        auto command_description = ParseCommandDescription(line);
        if (command_description) {
            commands_.push_back(std::move(command_description));
        }
    }

    void InputReader::ProcessStops(TransportCatalogue& catalogue, std::vector<std::tuple<std::string, std::string, int>>& distances) const {
        for (const auto& command : commands_) {
            if (command.command == "Stop") {
                auto coords_end = command.description.find("m to ");
                std::string coords_str;

                if (coords_end == std::string::npos) {
                    coords_str = command.description;
                } else {
                    coords_str = command.description.substr(0, command.description.rfind(',', coords_end));
                }

                coords_str = Trim(coords_str);

                Stop stop{ command.id, ParseCoordinates(coords_str) };
                catalogue.AddStop(stop.name, stop.coordinates);

                if (coords_end != std::string::npos) {
                    std::regex regex(R"((\d+)m to ([^,]+))");
                    std::smatch match;
                    std::string rest = command.description.substr(command.description.rfind(',', coords_end) + 1);

                    while (std::regex_search(rest, match, regex)) {
                        distances.emplace_back(command.id, match[2].str(), std::stoi(match[1]));
                        rest = match.suffix().str();
                    }
                }
            }
        }
    }
    void InputReader::ProcessBuses(TransportCatalogue& catalogue) const {
        for (const auto& command : commands_) {
            if (command.command == "Bus") {
                std::vector<std::string_view> stop_names = ParseRoute(command.description);
                bool is_circular = command.description.find('>') != std::string::npos;

                // Передаем название маршрута, список остановок и флаг кольцевого маршрута в метод AddBus
                catalogue.AddBus(command.id, std::vector<std::string>(stop_names.begin(), stop_names.end()), is_circular);
            }
        }
    }

    void InputReader::ProcessDistances(TransportCatalogue& catalogue, const std::vector<std::tuple<std::string, std::string, int>>& distances) const {
        for (const auto& [from, to, dist] : distances) {
            catalogue.SetDistance(from, to, dist);
        }
    }

    void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
        std::vector<std::tuple<std::string, std::string, int>> distances;
        ProcessStops(catalogue, distances);
        ProcessDistances(catalogue, distances);
        ProcessBuses(catalogue);
    }

}
