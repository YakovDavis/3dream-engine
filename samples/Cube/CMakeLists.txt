cmake_minimum_required(VERSION 3.26)
project(Cube)

set(CMAKE_CXX_STANDARD 20)

get_filename_component(D3E_ROOT_DIR "../../" ABSOLUTE)

add_executable(Cube WIN32
        src/CubeGame.cpp)

set_target_properties(Cube PROPERTIES
        WIN32_EXECUTABLE TRUE)

target_include_directories(Cube PUBLIC ${D3E_ROOT_DIR}/include)

add_library(D3E_LIB SHARED IMPORTED)
set_property(TARGET D3E_LIB PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${D3E_ROOT_DIR}/include
        IMPORTED_LOCATION ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.dll
        IMPORTED_IMPLIB ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.lib)
target_link_libraries(Cube "${D3E_LIB}")

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/Debug/D3Engined.dll)
file(COPY ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
