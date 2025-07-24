cmake_minimum_required(VERSION 3.10)
project(LongstaffSchwartz VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

find_package(PkgConfig REQUIRED)
pkg_search_module(OPENBLAS REQUIRED openblas)
find_package(nlohmann_json 3.2.0 REQUIRED)

find_path(LAPACKE_INCLUDE_DIR lapacke.h PATHS /usr/include /usr/include/x86_64-linux-gnu)
find_library(LAPACKE_LIB lapacke PATHS /usr/lib /usr/lib/x86_64-linux-gnu)
if(NOT LAPACKE_INCLUDE_DIR OR NOT LAPACKE_LIB)
    message(FATAL_ERROR "lapacke.h or liblapacke not found. Install liblapacke-dev.")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OPENBLAS_INCLUDE_DIRS}
    ${LAPACKE_INCLUDE_DIR}
)

add_library(core_objs OBJECT
    src/AssetSimulator.cpp
    src/OptionPricer.cpp
    src/LongstaffSchwartzEngine.cpp
    src/ConfigLoader.cpp)

target_link_libraries(core_objs PUBLIC
    ${OPENBLAS_LIBRARIES}
    ${LAPACKE_LIB}
    nlohmann_json::nlohmann_json)

add_executable(LongstaffSchwartz src/main.cpp $<TARGET_OBJECTS:core_objs>)

target_link_libraries(LongstaffSchwartz PRIVATE
    ${OPENBLAS_LIBRARIES}
    ${LAPACKE_LIB}
    nlohmann_json::nlohmann_json)

enable_testing()
add_executable(test_engine tests/test_engine.cpp $<TARGET_OBJECTS:core_objs>)

target_link_libraries(test_engine PRIVATE
    ${OPENBLAS_LIBRARIES}
    ${LAPACKE_LIB}
    nlohmann_json::nlohmann_json)

add_test(NAME EngineTests COMMAND test_engine)

