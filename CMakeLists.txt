cmake_minimum_required(VERSION 3.26)
project(engine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/assimp)
add_subdirectory(libs/DirectXTK12)
add_subdirectory(libs/nvrhi)
add_subdirectory(libs/EASTL)
add_subdirectory(libs/entt)

get_filename_component(EASTL_ROOT_DIR "libs/EASTL" ABSOLUTE)

set(EASTL_LIBRARY ${EASTL_ROOT_DIR}/cmake-build-debug/EASTL.lib) # TODO: fix for all configs

add_executable(engine src/core/Game.cpp
        src/core/Game.h
        src/core/ecs/WorldEntities.cpp
        src/core/ecs/WorldEntities.h
        src/core/ecs/EntityArchetype.cpp
        src/core/ecs/EntityArchetype.h
        src/core/ecs/ComponentContainer.cpp
        src/core/ecs/ComponentContainer.h
        src/core/ecs/AnonymousComponentContainer.h
        src/core/Common.h
        src/main.cpp
)

target_link_libraries(engine ${EASTL_LIBRARY}
        assimp
        DirectXTK12
        nvrhi
        entt
)
