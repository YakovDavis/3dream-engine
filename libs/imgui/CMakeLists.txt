cmake_minimum_required(VERSION 3.5)

project(imgui)

file(GLOB_RECURSE Public ./imgui/*.h)
list (FILTER Public EXCLUDE REGEX "backends.")
list (FILTER Public EXCLUDE REGEX "examples.")
file(GLOB_RECURSE Private ./imgui/*.cpp)
list (FILTER Private EXCLUDE REGEX "backends.")
list (FILTER Private EXCLUDE REGEX "examples.")
list (FILTER Private EXCLUDE REGEX "freetype.")

include_directories(./imgui)

add_library(imgui STATIC ${Public} ${Private})

target_include_directories(imgui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
        $<INSTALL_INTERFACE:include>
)