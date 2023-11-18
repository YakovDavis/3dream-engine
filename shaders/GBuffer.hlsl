#pragma pack_matrix(row_major)

struct VS_IN
{
	float4 pos : POSITION0;
    float4 normal : NORMAL0;
    float4 tangentU : TANGENT0;
	float4 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
    float4 tangentU : TANGENT;
    float4 tex : TEXCOORD;
	float4 worldPos : WORLDPOS;
};

struct GBuffer
{
	float4 DiffuseSpec : SV_Target0;
	float3 WorldPos : SV_Target1;
	float3 Normal : SV_Target2;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 gWorldView;
	float4x4 gInvTrWorldView;
};

Texture2D AlbedoMap : register(t0);
SamplerState AlbedoSampler : register(s0);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos.xyz, 1.0f), gWorldViewProj);
	output.tex = input.tex;
	output.normal = mul(float4(input.normal.xyz, 0.0f), gInvTrWorldView);
	output.tangentU = mul(float4(input.tangentU.xyz, 0.0f), gInvTrWorldView);
	output.worldPos = mul(float4(input.pos.xyz, 1.0f), gWorld);
	
	return output;
}

[earlydepthstencil]
GBuffer PSMain(PS_IN input) : SV_Target
{
	GBuffer result = (GBuffer)0;

	float4 objColor = AlbedoMap.SampleLevel(AlbedoSampler, input.tex.xy, 0);
	
	result.DiffuseSpec.xyz = objColor.xyz;
	result.DiffuseSpec.w = 0.5f;
	result.WorldPos = input.worldPos.xyz;
	result.Normal = normalize(input.normal.xyz);
	
	return result;
}
