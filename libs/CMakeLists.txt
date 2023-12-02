# Assimp
option(BUILD_SHARED_LIBS OFF)
option(ASSIMP_INSTALL OFF)
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

# FMOD
add_subdirectory(fmod)

# EASTL
add_subdirectory(eastl)

# Jolt
#add_subdirectory(JoltPhysics/Build)