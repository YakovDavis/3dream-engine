
#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

#include "EASTL/string.h"
#include "json_fwd.hpp"

#ifdef D3E_LIBRARY_EXPORTS
#define D3EAPI __declspec(dllexport)
#else
#define D3EAPI __declspec(dllimport)
#endif

#define D3E_APP_CORE \
	void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) \
	{ \
		return new uint8_t[size]; \
	} \
	void * __cdecl operator new[](unsigned __int64 size, unsigned __int64 flags, unsigned __int64 debugFlags, char const * name, int a,unsigned int b, char const* file, int line) \
	{ \
		return new uint8_t[size]; \
	}

using json = nlohmann::json;

namespace D3E
{
	using String = eastl::string;

	const String kCubeUUID = "21fc5f47-9db6-4f75-a9d3-cafa67dae071";
	const String kSphereUUID = "d88500fd-4976-4e34-bfe7-d077aaaffdc1";
	const String kCyllinderUUID = "06a0af19-1090-420a-badd-c9916a8e1739";
	const String kGeosphereUUID = "cb20b6c4-138e-499e-ab36-2572bd1db3da";
	const String kPlaneUUID = "630f8ccc-d809-4fcf-8eff-f00772e25c28";
	const String kGridUUID = "ef480c40-b6b7-49a6-b7cc-fffd443ba63d";
	const String kSkyboxMeshUUID = "ba806d98-cb91-4035-b2de-5b0085832c54";

	const String kDebugLineBindingSetUUID =
		"ef480c40-b6b7-49a6-b7cc-fffd443ba63d";

	const String kDebugLinesTextureUUID =
		"24c71f11-0d38-4594-ae18-c8eedca9b896";
	const String kWhiteTextureUUID = "34b9a6f1-240f-4d40-b76d-ad38ce9e65ea";
	const String kBlackTextureUUID = "eb93c841-6911-411c-93cd-54b24861e6e7";
	const String kNormalsDefaultTextureUUID = "c2346e38-a332-4c9f-bb91-f22591ce5f52";
	const String kDefaultGridMaterialUUID = "cf251109-d50a-49cb-912a-bbdfb5345c97";
	const String kEnvironmentScriptId = "591a9f8c-b5b7-4ce0-a9af-8bc809066222";

	const String kEnvTextureUUID = "4642c92a-a667-4df2-bf7a-b5f9cacfe281";
	const String kIrMapTextureUUID = "1029c59b-3ecb-4d88-b6a9-2cb0f7d65314";
	const String kSpBrdfLutTextureUUID = "75ed1f4e-e768-48ab-93b6-d911747f3136";
	const String kRenderDebugTextureUUID = "227b4fee-9094-4b34-87d7-d81b312c3819";

	const std::string kDefaultScriptName = "NewScript.lua";
	static const String kEmptyGoalName = "Empty Goal";
} // namespace D3E
#endif // COMMON_HEADER_H
