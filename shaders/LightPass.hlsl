#pragma pack_matrix(row_major)

#include "LightPassCommon.hlsl"

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 4
#endif

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float2 tex : TEXCOORD;
};
cbuffer cbPerScene : register(b0)
{
	float4 gLightDir;
	float4 gLightColor; // w = is shadow casting
	float4 gEyePosition;
	float4x4 gView;
};

cbuffer cbCascade : register(b1)
{
	float4x4 gViewProj[CASCADE_COUNT + 1];
	float4 gDistances;
};

Texture2D<float4> AlbedoBuffer : register(t0);
Texture2D<float3> PositionBuffer : register(t1);
Texture2D<float3> NormalBuffer : register(t2);
Texture2D<float3> MetalRoughnessBuffer : register(t3);
Texture2DArray CascadeShadowMap : register(t4);

SamplerState DefaultSampler : register(s0);
SamplerState DepthSampler : register(s1);

PS_IN VSMain(uint id: SV_VertexID)
{
	PS_IN output = (PS_IN)0;

	output.tex = float2(id & 1, (id & 2) >> 1);
	output.pos = float4(output.tex * float2(2, -2) + float2(-1, 1), 0, 1);
	
	return output;
}

float ShadowCalculation(float3 posWorldSpace, float4 posViewSpace, float dotN)
{
	float depthValue = abs(posViewSpace.z);

	int layer = -1;
	for (int i = 0; i < CASCADE_COUNT; ++i)
	{
		if (depthValue < gDistances[i])
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = CASCADE_COUNT;
	}

	float4 posLightSpace = mul(float4(posWorldSpace.xyz, 1.0), gViewProj[layer]);
	float3 projCoords = posLightSpace.xyz / posLightSpace.w;

	projCoords.x = (projCoords.x + 1.0f) / 2.0f;
	projCoords.y = (- projCoords.y + 1.0f) / 2.0f;
	float currentDepth = projCoords.z;

	if (currentDepth > 1.0f)
	{
		return 0.0f;
	}

	float bias = max(0.02f * (1.0f - dotN), 0.005f);
	const float biasModifier = 0.5f;
	if (layer == CASCADE_COUNT)
	{
		bias *= 1 / (1000.0 * biasModifier);
	}
	else
	{
		bias *= 1 / (gDistances[layer] * biasModifier);
	}

	// PCF
	float shadow = 0.0f;
	float2 texelSize = 1.0f / 2048.0f;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			shadow += CascadeShadowMap.Sample(DepthSampler, float3(projCoords.xy + float2(x, y) * texelSize, layer)).r < currentDepth - bias;
		}
	}
	shadow /= 9.0f;

	return shadow;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float3 norm = normalize(NormalBuffer.Load(int3(input.pos.xy, 0)));

    // Load input textures to get shading model params.
	float3 albedo = AlbedoBuffer.Load(int3(input.pos.xy, 0)).rgb;
	float metalness = MetalRoughnessBuffer.Load(int3(input.pos.xy, 0)).r;
	float roughness = MetalRoughnessBuffer.Load(int3(input.pos.xy, 0)).g;
	float3 worldPos = PositionBuffer.Load(int3(input.pos.xy, 0)).xyz;

	// Outgoing light direction (vector from world-space fragment position to the "eye").
	float3 Lo = normalize(gEyePosition.xyz - worldPos);
	
	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(norm, Lo));
		
	// Specular reflection vector.
	float3 Lr = 2.0 * cosLo * norm - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	float3 F0 = lerp(Fdielectric, albedo, metalness);

	// Direct lighting calculation.
	float3 directLighting = 0.0;
	float3 Li = -gLightDir.xyz;
	float3 Lradiance = gLightColor.rgb;

	// Half-vector between Li and Lo.
	float3 Lh = normalize(Li + Lo);

	// Calculate angles between surface normal and various light vectors.
	float cosLi = max(0.0, dot(norm, Li));
	float cosLh = max(0.0, dot(norm, Lh));

	// Calculate Fresnel term for direct lighting. 
	float3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
	// Calculate normal distribution for specular BRDF.
	float D = ndfGGX(cosLh, roughness);
	// Calculate geometric attenuation for specular BRDF.
	float G = gaSchlickGGX(cosLi, cosLo, roughness);

	// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
	// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
	float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);

	// Lambert diffuse BRDF.
	// We don't scale by 1/PI for lighting & material units to be more convenient.
	// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	float3 diffuseBRDF = kd * albedo;

	// Cook-Torrance specular microfacet BRDF.
	float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

	// Total contribution for this light.
	directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;

	float4 viewPos = mul(float4(worldPos, 1.0f), gView);

	[branch]
	if (gLightColor.w > 0)
	{
		directLighting *= (1 - ShadowCalculation(worldPos, viewPos, dot(norm, gLightDir.xyz)));
	}

	return float4(directLighting, 1.0);
}
