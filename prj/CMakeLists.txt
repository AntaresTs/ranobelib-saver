cmake_minimum_required(VERSION 3.16)
project(ranobelib-saver VERSION 0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(storage_structures INTERFACE)
target_include_directories(storage_structures INTERFACE include)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -g
    -O0
)

find_package(cpr REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(app.run main.cpp
    src/parsejson.cpp
    src/webparser.cpp
)
target_link_libraries(app.run PRIVATE
    storage_structures
    cpr::cpr
    CURL::libcurl
    nlohmann_json::nlohmann_json
)


