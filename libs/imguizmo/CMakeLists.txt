cmake_minimum_required(VERSION 3.5)

project(imguizmo)

file(GLOB_RECURSE Public ./imguizmo/*.h)
list (FILTER Public EXCLUDE REGEX "example.")
file(GLOB_RECURSE Private ./imguizmo/*.cpp)
list (FILTER Private EXCLUDE REGEX "example.")

include_directories(./imguizmo)
include_directories(../imgui/imgui)

add_library(imguizmo STATIC ${Public} ${Private})

target_include_directories(imguizmo
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imguizmo>
        $<INSTALL_INTERFACE:include>
)