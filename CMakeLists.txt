cmake_minimum_required(VERSION 3.26)
project(D3Engine)

set(CMAKE_DEBUG_POSTFIX d)

include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wv:18 /EHsc")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library("${PROJECT_NAME}" SHARED
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
        include/D3E/render/Material.h
        include/D3E/render/Material.h
        src/render/GBuffer.h
        src/render/GBuffer.cpp
        src/render/LightPass.cpp
        src/render/LightPass.h
        include/D3E/Components/render/LightComponent.h
        src/render/systems/LightRenderSystem.cpp
        src/render/systems/LightRenderSystem.h
        src/render/LightConstBuffer.h
        src/render/systems/LightInitSystem.cpp
        src/render/systems/LightInitSystem.h
        src/render/CsmConstBuffer.h
        include/D3E/Timer.h
        src/timers/Timer.cpp 
        include/D3E/TimerManager.h
        src/timers/TimerManager.cpp
        include/D3E/TimerHandle.h 
        src/timers/TimerHandle.cpp
        src/render/components/GridComponent.h
        src/render/systems/EditorUtilsRenderSystem.cpp
        src/render/systems/EditorUtilsRenderSystem.h
        src/editor/EditorConsole.cpp
        src/editor/EditorConsole.h
        src/editor/EditorConsole.cpp
        src/editor/EditorContentBrowser.cpp
        src/editor/EditorContentBrowser.h
        include/D3E/Components/ScriptComponent.h
        samples/ScriptingSandbox/src/ScriptingSandbox.h
        samples/ScriptingSandbox/src/ScriptingSandbox.cpp
        src/assetmng/ScriptMetaData.h
        src/assetmng/ScriptMetaData.cpp 
        src/assetmng/ScriptFactory.h
        src/assetmng/ScriptFactory.cpp
        src/assetmng/ScriptData.h
        include/D3E/scripting/ScriptingEngine.h
        src/scripting/ScriptingEngine.cpp
        src/scripting/EngineTypeBindings.h
        src/engine/systems/ScriptUpdateSystem.h
        src/engine/systems/ScriptUpdateSystem.cpp
        src/engine/systems/ScriptInitSystem.h
        src/engine/systems/ScriptInitSystem.cpp
        include/D3E/Components/PhysicsComponent.h
        src/engine/systems/PhysicsInitSystem.h
        src/engine/systems/PhysicsInitSystem.cpp
        src/physics/ObjectLayers.h
        src/engine/systems/PhysicsUpdateSystem.h
        src/physics/PhysicsInfo.h
        src/physics/PhysicsInfo.cpp
        src/engine/systems/PhysicsUpdateSystem.cpp
        include/D3E/Components/MouseComponent.h
        src/render/systems/InputSyncSystem.cpp
        src/render/systems/InputSyncSystem.h
        src/editor/EditorIdManager.cpp
        src/editor/EditorIdManager.h
        src/render/PickConstBuffer.h
        src/assetmng/MaterialFactory.cpp
        src/assetmng/MaterialFactory.h
        src/assetmng/Material.cpp
        src/render/DebugRenderer.cpp
        src/render/DebugRenderer.h
        src/physics/JoltDebugRenderer.cpp
        src/physics/JoltDebugRenderer.h
)

add_subdirectory(libs)

target_include_directories("${PROJECT_NAME}" PUBLIC libs/sol)
target_include_directories("${PROJECT_NAME}" PRIVATE libs/stb libs/nlohmann)
target_include_directories("${PROJECT_NAME}" PUBLIC libs/uuid_v4)


target_include_directories("${PROJECT_NAME}" PRIVATE libs/DirectXTK12/Inc)
target_include_directories("${PROJECT_NAME}" PRIVATE libs/DirectXTK12/Src)

target_include_directories("${PROJECT_NAME}" PUBLIC include)
target_include_directories("${PROJECT_NAME}" PRIVATE src)

if(WIN32)
    target_compile_definitions("${PROJECT_NAME}" PUBLIC D3E_PLATFORM_WINDOWS=1)
    target_compile_definitions("${PROJECT_NAME}" PUBLIC NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries("${PROJECT_NAME}" PUBLIC
        assimp
        nvrhi
        nvrhi_d3d12
        imgui
        FmodCore
        FmodStudio
        EASTL
        Jolt        
        lua_static
)


set(NVRHI_DEFINES "")

if (WIN32)
    set(NVRHI_DEFINES ${NVRHI_DEFINES} NOMINMAX VK_USE_PLATFORM_WIN32_KHR=1)
endif()

target_compile_definitions(nvrhi PRIVATE ${NVRHI_DEFINES})
target_compile_definitions(nvrhi_d3d12 PRIVATE ${NVRHI_DEFINES})
target_compile_definitions("${PROJECT_NAME}" PRIVATE ${NVRHI_DEFINES})

set(SHADER_BULID_BATCH)
file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/CompileShaders.bat" SHADER_BULID_BATCH)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
    COMMAND "${SHADER_BULID_BATCH}" "${CMAKE_SOURCE_DIR}"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        "${CMAKE_BINARY_DIR}/samples/Ducks"
        COMMENT "Copying ${FMOD_CORE_BINARY_PATH_DEBUG} to build folder"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        "${CMAKE_BINARY_DIR}/samples/scriptingsandbox"
        COMMENT "Copying ${FMOD_CORE_BINARY_PATH_DEBUG} to build folder"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"        
        "${CMAKE_BINARY_DIR}/samples/NewCube"
        COMMENT "Copying ${FMOD_CORE_BINARY_PATH_DEBUG} to build folder"
)

add_subdirectory(samples/ducks)
add_subdirectory(samples/scriptingsandbox)
add_subdirectory(samples/NewCube)
