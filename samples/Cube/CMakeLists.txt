cmake_minimum_required(VERSION 3.26)
project(Cube)

set(CMAKE_CXX_STANDARD 20)

get_filename_component(D3E_ROOT_DIR "../../" ABSOLUTE)

add_executable(${PROJECT_NAME} WIN32
        src/CubeGame.cpp
        src/CubeGame.h)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE)

target_include_directories(${PROJECT_NAME} PUBLIC ${D3E_ROOT_DIR}/include)

add_library(D3E_LIB SHARED IMPORTED)
set_target_properties(D3E_LIB PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${D3E_ROOT_DIR}/include"
  IMPORTED_LOCATION "${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.dll"
  IMPORTED_IMPLIB "${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.lib")

get_filename_component(EASTL_ROOT_DIR "${D3E_ROOT_DIR}/libs/EASTL" ABSOLUTE)
set(EASTL_LIBRARY debug ${EASTL_ROOT_DIR}/build/Debug/Debug/EASTL.lib optimized ${EASTL_ROOT_DIR}/build/Release/EASTL.lib)

target_link_libraries(${PROJECT_NAME} ${D3E_LIB})
target_link_libraries(${PROJECT_NAME} ${EASTL_LIBRARY})

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/Debug/D3Engined.dll)
file(COPY ${D3E_ROOT_DIR}/build/Debug/Debug/D3Engined.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/Debug/Shaders/)
file(COPY ${D3E_ROOT_DIR}/build/Debug/Debug/Shaders/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/Shaders/)

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/Debug/Textures/)
file(COPY ${D3E_ROOT_DIR}/textures/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/Textures/)

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/Debug/Textures/)
file(COPY textures/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/Textures/)