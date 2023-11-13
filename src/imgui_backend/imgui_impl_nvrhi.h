// dear imgui: Renderer Backend for nvrhi
// This needs to be used along with a Platform Backend (e.g. Win32)

#pragma once
#include "imgui.h"
#ifndef IMGUI_DISABLE

IMGUI_IMPL_API bool     ImGui_ImplNVRHI_Init();
IMGUI_IMPL_API void     ImGui_ImplNVRHI_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplNVRHI_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplNVRHI_RenderDrawData();

// Use if you want to reset your rendering device without losing Dear ImGui state.
IMGUI_IMPL_API void     ImGui_ImplNVRHI_InvalidateDeviceObjects();
IMGUI_IMPL_API bool     ImGui_ImplNVRHI_CreateDeviceObjects();

#endif // #ifndef IMGUI_DISABLE
