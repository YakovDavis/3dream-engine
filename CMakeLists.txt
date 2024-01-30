cmake_minimum_required(VERSION 3.26)
project(D3Engine)

set(CMAKE_DEBUG_POSTFIX d)

include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wv:18 /EHsc /bigobj")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

add_compile_options(/bigobj)

add_library("${PROJECT_NAME}" SHARED)

target_sources("${PROJECT_NAME}"
        PUBLIC
        include/D3E/CommonHeader.h
        include/D3E/EntryPoint.h
        include/D3E/App.h
        include/D3E/Game.h
        include/D3E/Debug.h
        include/D3E/components/render/StaticMeshComponent.h
        include/D3E/components/render/CameraComponent.h
        include/D3E/Components/TransformComponent.h
        include/D3E/Components/FPSControllerComponent.h
        include/D3E/Components/sound/SoundComponent.h
        include/D3E/AssetManager.h
        include/D3E/Components/ObjectInfoComponent.h
        include/D3E/systems/CreationSystems.h
        include/D3E/Components/sound/SoundListenerComponent.h
        include/SimpleMath.h
        include/SimpleMath.inl
        include/D3E/render/Material.h
        include/D3E/render/Material.h
        include/D3E/Uuid.h
        include/D3E/engine/ConsoleManager.h
        include/D3E/engine/ConsoleVariable.h
        include/D3E/engine/ConsoleVariable.h
        include/D3E/engine/IntConsoleVariable.h
        include/D3E/engine/FloatConsoleVariable.h
        include/D3E/Timer.h
        include/D3E/TimerManager.h
        include/D3E/Components/render/LightComponent.h
        include/D3E/TimerHandle.h
        include/D3E/scripting/ScriptingEngine.h
        include/D3E/Components/ScriptComponent.h
        include/D3E/Components/BaseComponent.h
        include/D3E/Components/MouseComponent.h
        include/D3E/Components/PhysicsComponent.h
        include/D3E/Components/PhysicsCharacterComponent.h
        include/D3E/Components/navigation/NavmeshComponent.h
        include/D3E/Components/render/SkyboxComponent.h
        include/D3E/Components/navigation/NavmeshConfig.h
        include/D3E/Components/navigation/NavmeshCore.h
        include/D3E/Components/TPSControllerComponent.h
        include/D3E/Components/AiComponent.h
        PRIVATE
        src/core/Game.cpp
        src/render/GameRender.cpp
        src/render/GameRender.h
        src/render/GameRenderD3D12.cpp
        src/render/GameRenderD3D12.h
        src/render/D3dUtil.h
        src/render/D3dUtil.cpp
        src/utils/Debug.cpp
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
        src/ai/graph/Vertex.h
        src/core/magic_enum.hpp
        src/render/GeometryGenerator.cpp
        src/render/GeometryGenerator.h
        src/engine/systems/CreationSystems.cpp
        src/render/PerObjectConstBuffer.h
        src/render/systems/StaticMeshRenderSystem.cpp
        src/render/systems/StaticMeshRenderSystem.h
        src/assetmng/MeshData.h
        src/render/VertexIndexBufferInfo.h
        src/assetmng/MeshFactory.cpp
        src/assetmng/MeshFactory.h
        src/render/CameraUtils.cpp
        src/render/CameraUtils.h
        src/editor/EditorUtils.cpp
        src/editor/EditorUtils.h
        src/assetmng/AssetManager.cpp
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
        src/editor/Editor.cpp
        src/editor/Editor.h
        src/assetmng/DefaultAssetLoader.cpp
        src/core/SimpleMath.cpp
        src/assetmng/Texture2DMetaData.h
        src/assetmng/Texture2DMetaData.cpp
        src/utils/Uuid.cpp 
        src/engine/systems/ChildTransformSynchronizationSystem.h
        src/engine/systems/ChildTransformSynchronizationSystem.cpp
        src/engine/ConsoleManager.cpp
        src/assetmng/MeshMetaData.cpp
        src/assetmng/MeshMetaData.h
        src/assetmng/MeshMetaData.h
        src/render/GBuffer.h
        src/render/GBuffer.cpp
        src/render/LightPass.cpp
        src/render/LightPass.h
        src/render/systems/LightRenderSystem.cpp
        src/render/systems/LightRenderSystem.h
        src/render/LightConstBuffer.h
        src/render/systems/LightInitSystem.cpp
        src/render/systems/LightInitSystem.h
        src/render/CsmConstBuffer.h
        src/timers/Timer.cpp
        src/timers/TimerManager.cpp
        src/timers/TimerHandle.cpp
        src/render/components/GridComponent.h
        src/render/systems/EditorUtilsRenderSystem.cpp
        src/render/systems/EditorUtilsRenderSystem.h
        src/editor/EditorConsole.cpp
        src/editor/EditorConsole.h
        src/editor/EditorConsole.cpp
        src/editor/EditorContentBrowser.cpp
        src/editor/EditorContentBrowser.h
        src/assetmng/ScriptMetaData.h
        src/assetmng/ScriptMetaData.cpp 
        src/assetmng/ScriptFactory.h
        src/assetmng/ScriptFactory.cpp
        src/assetmng/ScriptData.h
        src/scripting/ScriptingEngine.cpp
        src/scripting/EngineTypeBindings.h
        src/engine/systems/ScriptUpdateSystem.h
        src/engine/systems/ScriptUpdateSystem.cpp
        src/engine/systems/ScriptInitSystem.h
        src/engine/systems/ScriptInitSystem.cpp
        src/engine/systems/PhysicsInitSystem.h
        src/engine/systems/PhysicsInitSystem.cpp
        src/physics/ObjectLayers.h
        src/engine/systems/PhysicsUpdateSystem.h
        src/physics/PhysicsInfo.h
        src/physics/PhysicsInfo.cpp
        src/engine/systems/PhysicsUpdateSystem.cpp
        src/render/systems/InputSyncSystem.cpp
        src/render/systems/InputSyncSystem.h
        src/editor/EditorIdManager.cpp
        src/editor/EditorIdManager.h
        src/render/PickConstBuffer.h
        src/engine/systems/CharacterInitSystem.h
        src/engine/systems/CharacterInitSystem.cpp
        src/assetmng/MaterialFactory.cpp
        src/assetmng/MaterialFactory.h
        src/assetmng/Material.cpp
        src/render/DebugRenderer.cpp
        src/render/DebugRenderer.h
        src/physics/JoltDebugRenderer.cpp
        src/physics/JoltDebugRenderer.h
        src/assetmng/SoundMetaData.cpp
        src/assetmng/SoundMetaData.h
        src/engine/PrefabMetadata.h
        src/components/ObjectInfoComponent.cpp
        src/components/TransformComponent.cpp
        src/components/render/StaticMeshComponent.cpp
        src/components/render/CameraComponent.cpp
        src/components/render/LightComponent.cpp
        src/components/sound/SoundComponent.cpp
        src/components/sound/SoundListenerComponent.cpp
        src/components/FPSControllerComponent.cpp
        src/components/MouseComponent.cpp
        src/components/PhysicsCharacterComponent.cpp
        src/components/ScriptComponent.cpp
        src/components/PhysicsComponent.cpp
        src/physics/JoltContactListener.h
        src/physics/JoltContactListener.h
        src/physics/JoltContactListener.cpp
        src/core/EngineState.cpp
        src/core/EngineState.h
        src/utils/ECSUtils.h
        src/utils/ECSUtils.cpp
        src/scripting/LuaECSAdapter.h
        src/scripting/LuaECSAdapter.cpp
        src/engine/ComponentFactory.cpp
        src/engine/ComponentFactory.h
        src/assetmng/CDialogEventHandler.cpp
        src/assetmng/CDialogEventHandler.h
        src/render/RenderUtils.cpp
        src/render/RenderUtils.h
        src/editor/MaterialEditor.cpp
        src/editor/MaterialEditor.h
        src/editor/ComponentCreationWindow.h
        src/editor/ComponentCreationWindow.cpp
        src/utils/FilenameUtils.cpp
        src/utils/FilenameUtils.h
        src/render/PbrUtils.cpp
        src/render/PbrUtils.h
        src/tinyexr/miniz.c
        src/tinyexr/miniz.h
        src/tinyexr/tinyexr.h
        src/render/SkyboxCB.h
        src/assetmng/SkyboxVertex.h
        src/components/render/SkyboxComponent.cpp
        src/rmlui_backend/rmlui_backend.cpp
        src/rmlui_backend/rmlui_backend.h
        src/game_ui/GameUi.cpp
        src/game_ui/GameUi.h
        src/rmlui_backend/RmlUi_Platform_Win32.cpp
        src/rmlui_backend/RmlUi_Platform_Win32.h
        src/rmlui_backend/RmlUi_Include_Windows.h
        src/render/Csm.cpp
        src/render/Csm.h
        src/navigation/Context.h 
        src/navigation/Context.cpp 
        src/navigation/PerformanceTimer.h 
        src/navigation/PerformanceTimer.cpp
        src/navigation/NavmeshBuilder.h
        src/navigation/NavmeshBuilder.cpp
        src/components/NavmeshComponent.cpp
        src/utils/MeshUtils.h
        src/scripting/type_adapters/InfoAdapter.h
        src/RmlSolLua/RmlSolLua.h
        src/RmlSolLua/RmlSolLua.cpp
        src/RmlSolLua/bind/bind.cpp
        src/RmlSolLua/bind/bind.h
        src/RmlSolLua/bind/Colour.cpp
        src/RmlSolLua/bind/Context.cpp
        src/RmlSolLua/bind/Convert.cpp
        src/RmlSolLua/bind/DataModel.cpp
        src/RmlSolLua/bind/Document.cpp
        src/RmlSolLua/bind/Element.cpp
        src/RmlSolLua/bind/ElementDerived.cpp
        src/RmlSolLua/bind/ElementForm.cpp
        src/RmlSolLua/bind/Event.cpp
        src/RmlSolLua/bind/Global.cpp
        src/RmlSolLua/bind/Log.cpp
        src/RmlSolLua/bind/Vector.cpp
        src/RmlSolLua/plugin/SolLuaDataModel.cpp
        src/RmlSolLua/plugin/SolLuaDataModel.h
        src/RmlSolLua/plugin/SolLuaDocument.cpp
        src/RmlSolLua/plugin/SolLuaDocument.h
        src/RmlSolLua/plugin/SolLuaEventListener.cpp
        src/RmlSolLua/plugin/SolLuaEventListener.h
        src/RmlSolLua/plugin/SolLuaInstancer.cpp
        src/RmlSolLua/plugin/SolLuaInstancer.h
        src/RmlSolLua/plugin/SolLuaPlugin.cpp
        src/RmlSolLua/plugin/SolLuaPlugin.h
        src/engine/systems/TPSControllerSystem.cpp
        src/engine/systems/TPSControllerSystem.h
        src/scripting/type_adapters/InputDeviceAdapter.h
        src/scripting/type_adapters/InputDeviceAdapter.cpp
        src/scripting/type_adapters/PhysicsActivationAdapter.h
        src/scripting/type_adapters/PhysicsActivationAdapter.cpp
        src/components/TPSControllerComponent.cpp
        src/ai/State.h
        src/ai/Action.h
        src/ai/Agent.h 
        src/ai/Goal.h
        src/ai/structures/PriorityQueue.h
        src/ai/graph/Edge.h
        src/ai/GoapPlanner.h 
        src/components/AiComponent.cpp)

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

target_compile_definitions("${PROJECT_NAME}" PUBLIC D3E_WITH_EDITOR D3E_LIBRARY_EXPORTS)

set(WIN_FILE_LIBS "comctl32.lib propsys.lib shlwapi.lib")

target_link_libraries("${PROJECT_NAME}" PUBLIC
        assimp
        nvrhi
        nvrhi_d3d12
        imgui
        imguizmo
        FmodCore
        FmodStudio
        EASTL
        Jolt        
        lua_static
        ${WIN_FILE_LIBS}
        DebugUtils
        Detour
        DetourCrowd
        DetourTileCache
        Recast
        RmlUi::Core
)

set (DEFAULT_IMGUI_CONFIG_PATH "${CMAKE_SOURCE_DIR}/misc/imgui.ini")

set(NVRHI_DEFINES "")

if (WIN32)
    set(NVRHI_DEFINES ${NVRHI_DEFINES} NOMINMAX VK_USE_PLATFORM_WIN32_KHR=1)
endif()

target_compile_definitions(nvrhi PRIVATE ${NVRHI_DEFINES})
target_compile_definitions(nvrhi_d3d12 PRIVATE ${NVRHI_DEFINES})
target_compile_definitions("${PROJECT_NAME}" PRIVATE ${NVRHI_DEFINES})
target_compile_definitions("${PROJECT_NAME}" PUBLIC "JPH_PROFILE_ENABLED" "JPH_DEBUG_RENDERER")

set(SHADER_BULID_BATCH)
file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/CompileShaders.bat" SHADER_BULID_BATCH)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
    COMMAND "${SHADER_BULID_BATCH}" "${CMAKE_SOURCE_DIR}"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        "${CMAKE_BINARY_DIR}/samples/Ducks"
        COMMENT "Copying D3Engined.dll to build folder"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        "${CMAKE_BINARY_DIR}/samples/scriptingsandbox"
        COMMENT "Copying D3Engined.dll to build folder"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        "${CMAKE_BINARY_DIR}/samples/NewCube"
        COMMENT "Copying D3Engined.dll to build folder"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        "${CMAKE_BINARY_DIR}/samples/Character"
        COMMENT "Copying D3Engined.dll to build folder"
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/D3Engined.dll"
        "${CMAKE_BINARY_DIR}/samples/EmptyProject"
        COMMENT "Copying D3Engined.dll to build folder"
)

add_subdirectory(samples/ducks)
add_subdirectory(samples/scriptingsandbox)
add_subdirectory(samples/NewCube)
add_subdirectory(samples/Character)
add_subdirectory(samples/EmptyProject)