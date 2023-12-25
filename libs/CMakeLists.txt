# Assimp
option(BUILD_SHARED_LIBS OFF)
option(ASSIMP_INSTALL OFF)
option(ASSIMP_NO_EXPORT NO)
add_subdirectory(assimp)

# DirectXTK
option(BUILD_XAUDIO_WIN10 OFF)
add_subdirectory(DirectXTK12 ${CMAKE_BINARY_DIR}/bin/CMake/DirectXTK12)

# nvrhi
option(NVRHI_WITH_DX11 OFF)
option(NVRHI_INSTALL OFF)
option(NVRHI_WITH_VULKAN OFF)
add_subdirectory(nvrhi)

# imgui
add_subdirectory(imgui)

# imguizmo
add_subdirectory(imguizmo)

# FMOD
add_subdirectory(fmod)

# EASTL
add_subdirectory(eastl)

# Jolt
option(OVERRIDE_CXX_FLAGS OFF)
option(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
add_subdirectory(JoltPhysics/Build)

# lua
option(LUA_BUILD_AS_CXX ON)
option(LUA_ENABLE_SHARED OFF)
option(LUA_ENABLE_TESTING OFF)
add_subdirectory(lua)