cmake_minimum_required(VERSION 3.26)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 20)

get_filename_component(D3E_ROOT_DIR "../../" ABSOLUTE)

add_executable(HelloWorld WIN32 src/HelloWorldApp.h
        src/HelloWorldApp.cpp)

set_target_properties(HelloWorld PROPERTIES
        WIN32_EXECUTABLE TRUE)

target_include_directories(HelloWorld PUBLIC ${D3E_ROOT_DIR}/include)

add_library(D3E_LIB SHARED IMPORTED)
set_property(TARGET D3E_LIB PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${D3E_ROOT_DIR}/include
        IMPORTED_LOCATION ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.dll
        IMPORTED_IMPLIB ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.lib)
target_link_libraries(HelloWorld "${D3E_LIB}")

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/Debug/D3Engined.dll)
file(COPY ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)