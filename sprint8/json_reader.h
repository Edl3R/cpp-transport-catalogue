#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "svg.h"
#include <vector>

namespace json_reader {

    struct RenderSettings {
        double width = 0;
        double height = 0;
        double padding = 0;
        double line_width = 0;
        double stop_radius = 0;
        svg::Point bus_label_offset = { 0, 0 };
        svg::Point stop_label_offset = { 0, 0 };
        int bus_label_font_size = 0;
        int stop_label_font_size = 0;
        svg::Color underlayer_color = "none";
        double underlayer_width = 0;
        std::vector<svg::Color> color_palette;
    };

    class JsonReader {
    public:
        JsonReader(transport_catalogue::TransportCatalogue& tc) : tc_(tc) {}

        json::Node ProcessRequests(const json::Node& input);


    private:
        RenderSettings ParseRenderSettings(const json::Dict& dict);
        void ProcessBaseRequests(const json::Array& base_requests);
        json::Array ProcessStatRequests(const json::Array& stat_requests);

        transport_catalogue::TransportCatalogue& tc_;
        RenderSettings render_settings_;
    };

}  // namespace json_reader