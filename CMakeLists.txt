cmake_minimum_required(VERSION 3.26)
project(D3DreamEngine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/assimp)
add_subdirectory(libs/DirectXTK12)
add_subdirectory(libs/nvrhi)
add_subdirectory(libs/entt)

get_filename_component(EASTL_ROOT_DIR "libs/EASTL" ABSOLUTE)
include_directories (${EASTL_ROOT_DIR}/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAAssert/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EABase/include/Common)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAMain/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAStdC/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EATest/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAThread/include)
set(EASTL_LIBRARY debug ${EASTL_ROOT_DIR}/build/Debug/EASTL.lib optimized ${EASTL_ROOT_DIR}/build/Release/EASTL.lib)

add_executable(D3DreamEngine src/core/Game.cpp
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

target_link_libraries(D3DreamEngine PUBLIC
        ${EASTL_LIBRARY}
        assimp
        DirectXTK12
        nvrhi
        EnTT::EnTT
)
