//static const float gamma     = 2.2;
//static const float exposure  = 1.0;
static const float pureWhite = 1.0;


struct Constants
{
    float gamma;
    float exposure;
};
#ifdef SPIRV
[[vk::push_constant]] ConstantBuffer<Constants> gConst;
#else
cbuffer gConst : register(b0) { Constants gConst; }
#endif

float4 tonemap_reinhard(in float3 color)
{
   color *= 16;
   color = color/(1+color);
   float3 ret = pow(color, gConst.gamma);
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

    float3 ret = pow(abs(ccolor), gConst.gamma);

    return ret;
}

float3 tonemap_filmic(in float3 color)
{
    color = max(0, color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f)+ 0.06f);

    // result has 1/2.2 baked in
    return pow(color, gConst.exposure);
}

//----------------- UE4 filmic
// Copyright 2021 Dmitry Molchanov and Julia Molchanova
// This code is licensed under the MIT license
float3 tonemap_ue4_filmic(in float3 color)
{
    const float3x3 PRE_TONEMAPPING_TRANSFORM =
    {
        0.575961650,  0.344143820,  0.079952030,
        0.070806820,  0.827392350,  0.101774690,
        0.028035252,  0.131523770,  0.840242300
    };
    const float3x3 POST_TONEMAPPING_TRANSFORM =
    {
        1.666954300, -0.601741150, -0.065202855,
        -0.106835220,  1.237778600, -0.130948950,
        -0.004142626, -0.087411870,  1.091555000
    };

    // Narkowicz 2016, "ACES Filmic Tone Mapping Curve"
    // https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    
    const float3x3 EXPOSED_PRE_TONEMAPPING_TRANSFORM = gConst.exposure * PRE_TONEMAPPING_TRANSFORM;

    // Transform color spaces, perform blue correction and pre desaturation
    float3 WorkingColor = mul(EXPOSED_PRE_TONEMAPPING_TRANSFORM, color);

    // Apply tonemapping curve
    WorkingColor = saturate((WorkingColor * (a * WorkingColor + b)) / (WorkingColor * (c * WorkingColor + d) + e));

    // Transform color spaces, apply blue correction and post desaturation
    return mul( POST_TONEMAPPING_TRANSFORM, WorkingColor );
}
//----------------- UE4 filmic end


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
	float3 color = sceneColor.Sample(defaultSampler, pin.texcoord).rgb * gConst.exposure;
	
	float3 mappedColor = tonemap_ue4_filmic(color).xyz; //tonemap_filmic(color).xyz;

	return float4(mappedColor, 1.0);
}
