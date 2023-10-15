cmake_minimum_required(VERSION 3.26)
project(D3Engine)

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

add_library(D3Engine STATIC
        src/core/Common.h
        src/EntryPoint.cpp
        include/App.h
        src/core/App.cpp
        src/core/Game.cpp
        include/Game.h
        src/render/GameRender.cpp
        src/render/GameRender.h
        src/render/GameRenderD3D12.cpp
        src/render/GameRenderD3D12.h
)

target_include_directories(D3Engine PRIVATE include)
target_include_directories(D3Engine PRIVATE src)

target_link_libraries(D3Engine PUBLIC
        ${EASTL_LIBRARY}
        assimp
        DirectXTK12
        nvrhi
        EnTT::EnTT
)
