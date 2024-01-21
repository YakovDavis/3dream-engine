static const float gamma     = 2.2;
static const float exposure  = 1.0;
static const float pureWhite = 1.0;

float4 tonemap_reinhard(in float3 color)
{
   color *= 16;
   color = color/(1+color);
   float3 ret = pow(color, exposure); // gamma
   return float4(ret,1);
}

float3 tonemap_uncharted2(in float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float3 tonemap_uc2(in float3 color)
{
    float W = 11.2;

    color *= 16;  // Hardcoded Exposure Adjustment

    float exposure_bias = 2.0f;
    float3 curr = tonemap_uncharted2(exposure_bias*color);

    float3 white_scale = 1.0f/tonemap_uncharted2(W);
    float3 ccolor = curr*white_scale;

    float3 ret = pow(abs(ccolor), exposure); // gamma

    return ret;
}

float3 tonemap_filmic(in float3 color)
{
    color = max(0, color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f)+ 0.06f);

    // result has 1/2.2 baked in
    return pow(color, exposure);
}


struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

Texture2D sceneColor: register(t0);
SamplerState defaultSampler : register(s0);

PixelShaderInput VSMain(uint vertexID : SV_VertexID)
{
	PixelShaderInput vout;

	if(vertexID == 0) {
		vout.texcoord = float2(1.0, -1.0);
		vout.position = float4(1.0, 3.0, 0.0, 1.0);
	}
	else if(vertexID == 1) {
		vout.texcoord = float2(-1.0, 1.0);
		vout.position = float4(-3.0, -1.0, 0.0, 1.0);
	}
	else {
		vout.texcoord = float2(1.0, 1.0);
		vout.position = float4(1.0, -1.0, 0.0, 1.0);
	}
	return vout;
}

float4 PSMain(PixelShaderInput pin) : SV_Target
{
	float3 color = sceneColor.Sample(defaultSampler, pin.texcoord).rgb * exposure;
	
	float3 mappedColor = tonemap_filmic(color).xyz;

	return float4(mappedColor, 1.0);
}
