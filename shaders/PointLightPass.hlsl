#pragma pack_matrix(row_major)

#include "LightPassCommon.hlsl"

struct VS_IN
{
	float4 pos : POSITION0;
    float4 normal : NORMAL0;
    float4 tangentU : TANGENT0;
	float3 bitangent : BITANGENT;
	float4 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
};

cbuffer cbPerVolume : register(b0)
{
	float4x4 gWorldViewProj;
};

cbuffer cbPerLight : register(b0)
{
	float4 gLightPos;
	float4 gLightColor;
	float4 gEyePosition;
};

Texture2D<float4> AlbedoBuffer : register(t0);
Texture2D<float3> PositionBuffer : register(t1);
Texture2D<float3> NormalBuffer : register(t2);
Texture2D<float3> MetalRoughnessBuffer : register(t3);

SamplerState DefaultSampler : register(s0);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), gWorldViewProj);
	return output;
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
	float3 Li = gLightPos.xyz - worldPos;
	float lightTravel = length(Li);
	Li = normalize(Li);
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
	directLighting *= 1.0f / (1.0f + lightTravel * lightTravel);
	return float4(directLighting, 1.0);
}
