#pragma once
#include "transport_catalogue.h"
#include <string>
#include <vector>
#include <optional>

namespace request_handler {

    struct BusDetails {
        std::string name;
    };

    class RequestHandler {
    public:
        RequestHandler(const transport_catalogue::TransportCatalogue& db);

        std::optional<std::vector<BusDetails>> GetBusesByStop(const std::string& stop_name) const;

        domain::BusInfo GetBusInfo(const std::string& bus_name) const;

    private:
        const transport_catalogue::TransportCatalogue& db_;
    };

} // namespace request_handler