cmake_minimum_required(VERSION 3.26)
project(D3Engine)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wv:18 /EHsc")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

option(BUILD_SHARED_LIBS ON)
option(ASSIMP_INSTALL ON)
add_subdirectory(libs/assimp)

# Workaround for DxTk compilation
#find_program(DIRECTX_DXC_TOOL DXC.EXE
#         REQUIRED HINTS ${DIRECTX_DXC_PATH})
option(BUILD_XAUDIO_WIN10 OFF)
add_subdirectory(libs/DirectXTK12)

add_subdirectory(libs/imgui)

option(NVRHI_BUILD_SHARED OFF)
option(NVRHI_WITH_NVAPI OFF)
option(NVRHI_WITH_RTXMU OFF)
option(NVRHI_WITH_SHADER_COMPILER ON)
add_subdirectory(libs/nvrhi)

include_directories(libs/nvrhi/include)

include_directories(libs/stb)

add_subdirectory(libs/FMOD)

include_directories(libs/DirectXTK12/Inc)
include_directories(libs/DirectXTK12/Src)

include_directories(libs/uuid_v4)
include_directories(libs/nlohmann)

#FetchContent_Declare(
#        JoltPhysics
#        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
#        GIT_TAG "0436c32dfd83d26f7559e260378d7fa1d7962841"
#        SOURCE_SUBDIR "Build"
#)
#FetchContent_MakeAvailable(JoltPhysics)

get_filename_component(EASTL_ROOT_DIR "libs/EASTL" ABSOLUTE)
set(EASTL_LIBRARY debug ${EASTL_ROOT_DIR}/build/Debug/Debug/EASTL.lib optimized ${EASTL_ROOT_DIR}/build/Release/EASTL.lib)

add_library(D3Engine SHARED
        include/D3E/CommonHeader.h
        include/D3E/EntryPoint.h
        include/D3E/App.h
        src/core/Game.cpp
        include/D3E/Game.h
        src/render/GameRender.cpp
        src/render/GameRender.h
        src/render/GameRenderD3D12.cpp
        src/render/GameRenderD3D12.h
        src/render/D3dUtil.h
        src/render/D3dUtil.cpp
        src/utils/Debug.cpp
        include/D3E/Debug.h
        src/render/DisplayWin32.cpp
        src/render/DisplayWin32.h
        src/core/App.cpp
        src/input/InputDevice.cpp
        src/input/InputDevice.h
        src/input/Keys.h
        src/core/Delegates.cpp
        src/core/Delegates.h
        src/render/Display.h
        src/render/ShaderFactory.cpp
        src/render/ShaderFactory.h
        src/render/NvrhiMessageCallback.h
        src/assetmng/TextureFactory.cpp
        src/assetmng/TextureFactory.h
        src/render/Vertex.h
        src/core/magic_enum.hpp
        include/D3E/components/render/StaticMeshComponent.h
        src/render/GeometryGenerator.cpp
        src/render/GeometryGenerator.h
        src/engine/systems/CreationSystems.cpp
        include/D3E/systems/CreationSystems.h
        src/render/PerObjectConstBuffer.h
        src/render/systems/StaticMeshRenderSystem.cpp
        src/render/systems/StaticMeshRenderSystem.h
        src/assetmng/MeshData.h
        src/render/VertexIndexBufferInfo.h
        src/assetmng/MeshFactory.cpp
        src/assetmng/MeshFactory.h
        src/render/CameraUtils.cpp
        src/render/CameraUtils.h
        include/D3E/components/render/CameraComponent.h
        include/D3E/Components/TransformComponent.h
        include/D3E/Components/FPSControllerComponent.h
        include/D3E/Components/sound/SoundComponent.h
        src/editor/EditorUtils.cpp
        src/editor/EditorUtils.h
        src/assetmng/AssetManager.cpp
        include/D3E/AssetManager.h
        include/D3E/Components/ObjectInfoComponent.h
        src/render/systems/StaticMeshInitSystem.cpp
        src/render/systems/StaticMeshInitSystem.h
        src/engine/systems/FPSControllerSystem.cpp
        src/engine/systems/FPSControllerSystem.h
        src/imgui_backend/imgui_nvrhi.cpp
        src/imgui_backend/imgui_nvrhi.h
        src/sound_engine/SoundEngine.h
        src/sound_engine/SoundEngineCommon.h
        src/sound_engine/SoundEngine.cpp
        src/engine/systems/SoundEngineListenerSystem.h
        src/engine/systems/SoundEngineListenerSystem.cpp
        include/D3E/Components/sound/SoundListenerComponent.h
        src/editor/Editor.cpp
        src/editor/Editor.h
        src/assetmng/DefaultAssetLoader.cpp
        src/core/SimpleMath.cpp
        include/SimpleMath.h
        include/SimpleMath.inl
        src/assetmng/Texture2DMetaData.h
        src/assetmng/Texture2DMetaData.cpp
        include/D3E/Uuid.h
        src/utils/Uuid.cpp 
        src/engine/systems/ChildTransformSynchronizationSystem.h
        src/engine/systems/ChildTransformSynchronizationSystem.cpp
        include/D3E/engine/ConsoleManager.h
        src/engine/ConsoleManager.cpp
        include/D3E/engine/ConsoleVariable.h
        include/D3E/engine/ConsoleVariable.h
        include/D3E/engine/IntConsoleVariable.h
        include/D3E/engine/FloatConsoleVariable.h

        src/assetmng/MeshMetaData.cpp
        src/assetmng/MeshMetaData.h
        src/assetmng/MeshMetaData.h
        include/D3E/Timer.h
        src/timers/Timer.cpp 
        include/D3E/TimerManager.h
        src/timers/TimerManager.cpp
        include/D3E/TimerHandle.h 
        src/timers/TimerHandle.cpp)

target_include_directories(D3Engine PUBLIC include)
target_include_directories(D3Engine PRIVATE src)
#target_include_directories(D3Engine PUBLIC ${JoltPhysics_SOURCE_DIR}/..)

if(WIN32)
    target_compile_definitions(D3Engine PUBLIC D3E_PLATFORM_WINDOWS=1)
    target_compile_definitions(D3Engine PUBLIC NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(D3Engine PUBLIC
        ${EASTL_LIBRARY}
        assimp
        nvrhi
        imgui
        FmodCore
        FmodStudio
        #Jolt        
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
