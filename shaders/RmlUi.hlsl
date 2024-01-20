#pragma pack_matrix(row_major)

struct VS_IN
{
	float4 pos : POSITION0;
    float4 col : COLOR0;
	float4 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
    float4 col : COLOR;
    float4 tex : TEXCOORD;
};

cbuffer cbUserData : register(b0)
{
	float4x4 gTransform;
    float4 gTranslate;
};

Texture2D UserTexture : register(t0);
SamplerState UserSampler : register(s0);

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN)0;
	
    float2 translatedPos = input.pos.xy + gTranslate.xy;
	output.pos = mul(float4(translatedPos, 0.0f, 1.0f), gTransform);
	output.tex = input.tex;
    output.col = input.col;
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 texColor = UserTexture.SampleLevel(UserSampler, input.tex.xy, 0);
	return texColor * input.col;
}
