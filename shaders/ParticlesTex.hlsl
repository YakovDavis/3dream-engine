#pragma pack_matrix(row_major)

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

float4 PSMain(PS_IN input) : SV_Target
{
    float4 objColor = DiffuseMap.SampleLevel(Sampler, input.tex.xy, 0);

    return objColor * 0.9f;
}
