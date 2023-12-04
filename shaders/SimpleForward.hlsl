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
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 gWorldView;
	float4x4 gInvTrWorldView;
};

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos.xyz, 1.0f), gWorldViewProj);
	output.normal = mul(float4(input.normal.xyz, 0.0f), gInvTrWorldView);
	output.tangentU = mul(float4(input.tangentU.xyz, 0.0f), gInvTrWorldView);
    output.tex = input.tex;
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 result = DiffuseMap.SampleLevel(Sampler, input.tex.xy, 0);;

	return float4(result.xyz, 1.0f);
}
