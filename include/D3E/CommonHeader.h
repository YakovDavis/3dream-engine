
#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

#include "EASTL/string.h"
#include "json_fwd.hpp"

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

	const String kDebugLineBindingSetUUID =
		"ef480c40-b6b7-49a6-b7cc-fffd443ba63d";

	const String kDebugLinesTextureUUID =
		"24c71f11-0d38-4594-ae18-c8eedca9b896";
	const String kWhiteTextureUUID = "34b9a6f1-240f-4d40-b76d-ad38ce9e65ea";
	const String kBlackTextureUUID = "eb93c841-6911-411c-93cd-54b24861e6e7";
	const String kNormalsDefaultTextureUUID = "c2346e38-a332-4c9f-bb91-f22591ce5f52";
	const String kDefaultGridMaterialUUID = "cf251109-d50a-49cb-912a-bbdfb5345c97";
	const String kEnvironmentScriptId = "591a9f8c-b5b7-4ce0-a9af-8bc809066222";
} // namespace D3E
#endif // COMMON_HEADER_H
