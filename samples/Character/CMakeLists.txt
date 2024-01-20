cmake_minimum_required(VERSION 3.26)
project(Character)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} WIN32
        src/CharacterGame.cpp
        src/CharacterGame.h)

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

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FREETYPE_BINARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying ${FREETYPE_BINARY_PATH} to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/shaders"
        "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    COMMENT "Copying shaders to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
    COMMENT "Copying assets to build folder"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
    COMMENT "Copying assets to build folder"
)
