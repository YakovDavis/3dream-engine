cmake_minimum_required(VERSION 3.26)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 20)

get_filename_component(D3E_ROOT_DIR "../../" ABSOLUTE)
set(D3E_LIBRARY debug ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.lib optimized ${D3E_ROOT_DIR}/build/Release/Release/D3Engine.lib)

add_executable(HelloWorld src/HelloWorldApp.h
        src/HelloWorldApp.cpp)

target_include_directories(HelloWorld PUBLIC ${D3E_ROOT_DIR}/include)

target_link_libraries(HelloWorld PUBLIC ${D3E_LIBRARY})
