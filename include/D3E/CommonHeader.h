
#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

#include "EASTL/string.h"

namespace D3E
{
	using String = eastl::string;

	const String kCubeUUID = "21fc5f47-9db6-4f75-a9d3-cafa67dae071";
	const String kSphereUUID = "d88500fd-4976-4e34-bfe7-d077aaaffdc1";
	const String kCyllinderUUID = "06a0af19-1090-420a-badd-c9916a8e1739";
	const String kGeosphereUUID = "cb20b6c4-138e-499e-ab36-2572bd1db3da";
	const String kPlaneUUID = "630f8ccc-d809-4fcf-8eff-f00772e25c28";
	const String kGridUUID = "ef480c40-b6b7-49a6-b7cc-fffd443ba63d";

	const String kDebugLineBindingSetUUID = "ef480c40-b6b7-49a6-b7cc-fffd443ba63d";

	const String kDebugLinesTextureUUID = "24c71f11-0d38-4594-ae18-c8eedca9b896";
	const String kWhiteTextureUUID = "34b9a6f1-240f-4d40-b76d-ad38ce9e65ea";
}

#endif // COMMON_HEADER_H
