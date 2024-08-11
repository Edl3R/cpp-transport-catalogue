cmake_minimum_required(VERSION 3.27)
project(sprint8)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(sprint8
        geo.h
        input_reader.cpp
        input_reader.h
        main.cpp
        stat_reader.cpp
        stat_reader.h
        transport_catalogue.cpp
        transport_catalogue.h)
