add_library(FmodCore SHARED IMPORTED GLOBAL)
set_target_properties(FmodCore PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/fmod_core/include"
  IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/fmod_core/lib/fmodL.dll"
  IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/fmod_core/lib/fmod.dll"
  IMPORTED_IMPLIB_DEBUG "${CMAKE_CURRENT_LIST_DIR}/fmod_core/lib/fmodL_vc.lib"
  IMPORTED_IMPLIB_RELEASE "${CMAKE_CURRENT_LIST_DIR}/fmod_core/lib/fmod_vc.lib"
)

add_library(FmodStudio SHARED IMPORTED GLOBAL)

set_target_properties(FmodStudio PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/fmod_studio/include"
  IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/fmod_studio/lib/fmodstudioL.dll"
  IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/fmod_studio/lib/fmodstudio.dll"
  IMPORTED_IMPLIB_DEBUG "${CMAKE_CURRENT_LIST_DIR}/fmod_studio/lib/fmodstudioL_vc.lib"
  IMPORTED_IMPLIB_RELEASE "${CMAKE_CURRENT_LIST_DIR}/fmod_studio/lib/fmodstudio_vc.lib"
)