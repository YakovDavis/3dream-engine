cmake_minimum_required(VERSION 3.26)
project(D3Engine)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(libs/assimp)
add_subdirectory(libs/DirectXTK12)

option(NVRHI_BUILD_SHARED OFF)
option(NVRHI_WITH_NVAPI OFF)
option(NVRHI_WITH_RTXMU OFF)
option(NVRHI_WITH_SHADER_COMPILER ON)
add_subdirectory(libs/nvrhi)

add_subdirectory(libs/entt)

include_directories(libs/nvrhi/include)

include_directories(libs/DirectXTK12/Inc)
include_directories(libs/DirectXTK12/Src)

get_filename_component(EASTL_ROOT_DIR "libs/EASTL" ABSOLUTE)
include_directories (${EASTL_ROOT_DIR}/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAAssert/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EABase/include/Common)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAMain/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAStdC/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EATest/include)
include_directories (${EASTL_ROOT_DIR}/test/packages/EAThread/include)
set(EASTL_LIBRARY debug ${EASTL_ROOT_DIR}/build/Debug/EASTL.lib optimized ${EASTL_ROOT_DIR}/build/Release/EASTL.lib)

add_library(D3Engine SHARED
        src/core/Common.h
        include/EntryPoint.h
        include/App.h
        src/core/Game.cpp
        include/Game.h
        src/render/GameRender.cpp
        src/render/GameRender.h
        src/render/GameRenderD3D12.cpp
        src/render/GameRenderD3D12.h
        src/render/D3dUtil.h
        src/render/D3dUtil.cpp
)

target_include_directories(D3Engine PUBLIC include)
target_include_directories(D3Engine PRIVATE src)

if(WIN32)
    target_compile_definitions(D3Engine PUBLIC D3E_PLATFORM_WINDOWS=1)
    target_compile_definitions(D3Engine PUBLIC NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(D3Engine PUBLIC
        ${EASTL_LIBRARY}
        assimp
        nvrhi
        EnTT::EnTT
)

set(NVRHI_DEFINES "")
if (WIN32)
    set(NVRHI_DEFINES ${NVRHI_DEFINES} NOMINMAX VK_USE_PLATFORM_WIN32_KHR=1)
endif()

set(NVRHI_DEFINES ${NVRHI_DEFINES} USE_DX12=1)
target_link_libraries( D3Engine PUBLIC nvrhi_d3d12 )

target_compile_definitions(nvrhi PRIVATE ${NVRHI_DEFINES})
target_compile_definitions(D3Engine PRIVATE ${NVRHI_DEFINES})

target_compile_definitions(nvrhi_d3d12 PRIVATE ${NVRHI_DEFINES})

install(TARGETS D3Engine
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
