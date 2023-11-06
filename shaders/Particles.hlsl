#pragma pack_matrix(row_major)

struct Particle
{
    float4 Position;
    float4 Velocity;
    float4 Color0;
    float2 Size0Size1;
    float LifeTime;
    float MaxLifeTime;
};

struct ParticleDepths
{
    uint Index;
    float Depth;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

cbuffer CB1 : register(b0)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProj;
    float4 gDeltaTimeMaxParticlesGroupdim;
    float4x4 gInvProj;
    float4 BoundingSphereInfo;
};

StructuredBuffer<Particle> renderBufSrc : register(t0);
StructuredBuffer<ParticleDepths> sortedBufSrc : register(t1);

PS_IN VSMain(uint vertexID: SV_VertexID)
{
    PS_IN output = (PS_IN)0;

    uint index = sortedBufSrc[vertexID / 4].Index;
	
    Particle p = renderBufSrc[index];
    output.pos = mul(p.Position, gWorld);
    output.pos = mul(output.pos, gView);
    output.color = p.Color0;
    output.tex = float2(0.0f, 0.0f);

    uint corner = vertexID % 4;

    float pSize = lerp(p.Size0Size1.x, p.Size0Size1.y, saturate(1.0f - p.LifeTime / p.MaxLifeTime));

    [flatten]
    if (corner == 0)
    {
        output.pos += float4(pSize, pSize, 0.0f, 0.0f);
        output.tex = float2(1.0f, 1.0f);
    }

    [flatten]
    if (corner == 1)
    {
        output.pos += float4(-pSize, -pSize, 0.0f, 0.0f);
        output.tex = float2(0.0f, 0.0f);
    }

    [flatten]
    if (corner == 2)
    {
        output.pos += float4(pSize, -pSize, 0.0f, 0.0f);
        output.tex = float2(0.0f, 1.0f);
    }

    [flatten]
    if (corner == 3)
    {
        output.pos += float4(-pSize, pSize, 0.0f, 0.0f);
        output.tex = float2(1.0f, 0.0f);
    }

    output.pos = mul(output.pos, gProj);
	
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float amount = length(input.tex - float2(0.5f, 0.5f)) * 2.0f;

    amount = smoothstep(0.0f, 1.0f, 1.0f - amount);

    return float4(input.color.rgb, amount);
}
