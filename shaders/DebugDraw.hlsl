#pragma pack_matrix(row_major)

#define MAX_BATCH_VERTEX_COUNT 16

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 col : COLOR;
};

struct Vertex
{
    float4 pos;
    float4 col;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 gWorldView;
	float4x4 gInvTrWorldView;
};

cbuffer cbDebugDraw : register(b1)
{
	Vertex v[MAX_BATCH_VERTEX_COUNT];
};

PS_IN VSMain(uint id: SV_VertexID)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(v[id].pos, gWorldViewProj);
	output.col = v[id].col;

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	return input.col;
}
