struct VS_IN
{
	float4 pos : POSITION0;
	float4 tex : TEXCOORD0;
	float4 normal : NORMAL0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 tex : TEXCOORD;
	float4 normal : NORMAL;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	
	output.pos = input.pos;
	output.tex = input.tex;
	output.normal = input.normal;
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}