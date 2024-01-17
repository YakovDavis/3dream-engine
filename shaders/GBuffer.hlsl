#pragma pack_matrix(row_major)

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
	float4 normal : NORMAL;
    float3x3 tangentBasis : TBASIS;
    float4 tex : TEXCOORD;
	float4 worldPos : WORLDPOS;
	int EditorId : EDITORID;
};

struct GBuffer
{
	float3 Albedo : SV_Target0;
	float3 WorldPos : SV_Target1;
	float3 Normal : SV_Target2;
	float3 MetalRoughnessSpecular : SV_Target3;
	int EditorIds : SV_Target4;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 gWorldView;
	float4x4 gInvTrWorldView;
	int EditorId;
};

Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetalnessMap : register(t2);
Texture2D RoughnessMap : register(t3);
TextureCube SpecularMap : register(t4);
TextureCube IrradianceMap : register(t5);
Texture2D SpecularBRDF_LUT : register(t6);

SamplerState DefaultSampler : register(s0);
SamplerState spBRDF_Sampler : register(s1);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos.xyz, 1.0f), gWorldViewProj);
	output.tex = input.tex;
	output.normal = mul(float4(input.normal.xyz, 0.0f), gInvTrWorldView);

	float3 tangent = mul(float4(input.tangentU.xyz, 0.0f), gInvTrWorldView).xyz;
	float3 bitangent = mul(float4(input.bitangent.xyz, 0.0f), gInvTrWorldView).xyz;

	float3x3 TBN = float3x3(tangent, bitangent, output.normal.xyz);
	output.tangentBasis = mul((float3x3)gInvTrWorldView, transpose(TBN));

	output.worldPos = mul(float4(input.pos.xyz, 1.0f), gWorld);

	output.EditorId = EditorId;
	
	return output;
}

[earlydepthstencil]
GBuffer PSMain(PS_IN input)
{
	GBuffer result = (GBuffer)0;

	float4 objColor = AlbedoMap.SampleLevel(DefaultSampler, input.tex.xy, 0);
	
	result.Albedo.xyz = objColor.xyz;
	result.MetalRoughnessSpecular.x = MetalnessMap.SampleLevel(DefaultSampler, input.tex.xy, 0).r;
	result.MetalRoughnessSpecular.y = RoughnessMap.SampleLevel(DefaultSampler, input.tex.xy, 0).r;
	result.MetalRoughnessSpecular.z = 0.5f; // spec
	result.WorldPos = input.worldPos.xyz;
	result.EditorIds = input.EditorId;

	float3 N = normalize(2.0 * NormalMap.SampleLevel(DefaultSampler, input.tex.xy, 0).rgb - 1.0);
	result.Normal = normalize(mul(input.tangentBasis, N));
	
	return result;
}
