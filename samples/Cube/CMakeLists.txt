cmake_minimum_required(VERSION 3.26)
project(Cube)

set(CMAKE_CXX_STANDARD 20)

get_filename_component(D3E_ROOT_DIR "../../" ABSOLUTE)
set(D3E_LIBRARY debug ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.lib optimized ${D3E_ROOT_DIR}/build/Release/Release/D3Engine.lib)

add_executable(Cube WIN32 src/CubeGame.h
        src/CubeGame.cpp)

target_include_directories(Cube PUBLIC ${D3E_ROOT_DIR}/include)

target_link_libraries(Cube PUBLIC ${D3E_LIBRARY})
