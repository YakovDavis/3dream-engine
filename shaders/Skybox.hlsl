#pragma pack_matrix(row_major)

// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Environment skybox.

cbuffer TransformConstants : register(b0)
{
	float4x4 skyProjectionMatrix;
};

struct PixelShaderInput
{
	float3 localPosition : POSITION;
	float4 pixelPosition : SV_POSITION;
};

TextureCube envTexture : register(t0);
SamplerState defaultSampler : register(s0);

// Vertex shader
PixelShaderInput VSMain(float3 position : POSITION)
{
	PixelShaderInput vout;
	vout.localPosition = position;
	vout.pixelPosition = mul(float4(position, 1.0), skyProjectionMatrix);
	return vout;
}

// Pixel shader
float4 PSMain(PixelShaderInput pin) : SV_Target
{
	float3 envVector = normalize(pin.localPosition);
	return envTexture.SampleLevel(defaultSampler, envVector, 0);
}
