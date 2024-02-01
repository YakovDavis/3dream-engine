#pragma pack_matrix(row_major)

#include "LightPassCommon.hlsl"

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float2 tex : TEXCOORD;
};

cbuffer cbPerScene : register(b0)
{
	float4 gEyePosition;
};

Texture2D<float4> AlbedoBuffer : register(t0);
Texture2D<float3> PositionBuffer : register(t1);
Texture2D<float3> NormalBuffer : register(t2);
Texture2D<float3> MetalRoughnessBuffer : register(t3);
TextureCube SpecularTexture : register(t4);
TextureCube IrradianceTexture : register(t5);
Texture2D SpecularBRDF_LUT : register(t6);

SamplerState DefaultSampler : register(s0);
SamplerState spBRDF_Sampler : register(s1);

// Returns number of mipmap levels for specular IBL environment map.
uint querySpecularTextureLevels()
{
	uint width, height, levels;
	SpecularTexture.GetDimensions(0, width, height, levels);
	return levels;
}

PS_IN VSMain(uint id: SV_VertexID)
{
	PS_IN output = (PS_IN)0;

	output.tex = float2(id & 1, (id & 2) >> 1);
	output.pos = float4(output.tex * float2(2, -2) + float2(-1, 1), 0, 1);
	
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

    // Ambient lighting (IBL).
	float3 ambientLighting;
	{
		// Sample diffuse irradiance at normal direction.
		float3 irradiance = IrradianceTexture.Sample(DefaultSampler, norm).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		float3 F = fresnelSchlick(F0, cosLo);

		// Get diffuse contribution factor (as with direct lighting).
		float3 kd = lerp(1.0 - F, 0.0, metalness);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		float3 diffuseIBL = kd * albedo * irradiance;

		// Sample pre-filtered specular reflection environment at correct mipmap level.
		uint specularTextureLevels = querySpecularTextureLevels();
		float3 specularIrradiance = SpecularTexture.SampleLevel(DefaultSampler, normalize(Lr), roughness * specularTextureLevels).rgb;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		float2 specularBRDF = SpecularBRDF_LUT.Sample(spBRDF_Sampler, float2(cosLo, roughness)).rg;

		// Total specular IBL contribution.
		float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// Total ambient lighting contribution.
		ambientLighting = diffuseIBL + specularIBL;
	}

	// Final fragment color.
	return float4(ambientLighting, 1.0);
}
