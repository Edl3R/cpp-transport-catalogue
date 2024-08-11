#include "stat_reader.h"
#include "transport_catalogue.h"
#include <iomanip>
#include <iostream>
#include <string_view>

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    if (request.substr(0, 3) == "Bus") {
        auto bus_name = std::string(request.substr(4));
        auto bus_info = transport_catalogue.GetBusInfo(bus_name);
        if (bus_info) {
            output << "Bus " << bus_name << ": "
                   << bus_info->stop_count << " stops on route, "
                   << bus_info->unique_stop_count << " unique stops, "
                   << bus_info->route_length << " route length\n";
        } else {
            output << "Bus " << bus_name << ": not found\n";
        }
    } else if (request.substr(0, 5) == "Stop ") {
        auto stop_name = std::string(request.substr(5));
        auto buses = transport_catalogue.GetBusesByStop(stop_name);
        if (buses) {
            if (buses->empty()) {
                output << "Stop " << stop_name << ": no buses\n";
            } else {
                output << "Stop " << stop_name << ": buses ";
                for (const auto& bus : *buses) {
                    output << bus << ' ';
                }
                output << '\n';
            }
        } else {
            output << "Stop " << stop_name << ": not found\n";
        }
    }
}