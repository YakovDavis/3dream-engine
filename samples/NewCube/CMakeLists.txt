cmake_minimum_required(VERSION 3.26)
project(NewCube)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} WIN32
        src/NewCubeGame.cpp
        src/NewCubeGame.h)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE)

target_link_libraries(${PROJECT_NAME} PRIVATE D3Engine)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying ${FMOD_CORE_BINARY_PATH_DEBUG} to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FMOD_CORE_BINARY_PATH_DEBUG}
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying ${FMOD_CORE_BINARY_PATH_DEBUG} to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FMOD_STUDIO_BINARY_PATH_DEBUG}
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying ${FMOD_STUDIO_BINARY_PATH_DEBUG} to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/textures"
        "${CMAKE_CURRENT_BINARY_DIR}/Textures"
    COMMENT "Copying ${FMOD_STUDIO_BINARY_PATH_DEBUG} to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        "${CMAKE_CURRENT_BINARY_DIR}/Textures"
    COMMENT "Copying ${FMOD_STUDIO_BINARY_PATH_DEBUG} to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/models"
        "${CMAKE_CURRENT_BINARY_DIR}/Models"
    COMMENT "Copying ${FMOD_STUDIO_BINARY_PATH_DEBUG} to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/Shaders"
        "${CMAKE_CURRENT_BINARY_DIR}/Shaders"
    COMMENT "Copying ${FMOD_STUDIO_BINARY_PATH_DEBUG} to build folder"
)
