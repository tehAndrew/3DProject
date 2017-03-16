Texture2D DiffuseMap : register(t0);
SamplerState samplerState : register(s0);

cbuffer Material : register(b0) {
	float4 ReflectiveColor;
	float  Gloss;
}

struct GS_OUT {
	float4 PosWVP   : SV_POSITION;
	float4 PosW     : POSITION;
	float3 NormalW  : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PS_OUT {
	float4 Color    : SV_TARGET0;
	float4 PosWorld : SV_TARGET1;
	float4 Normal   : SV_TARGET2;
	float4 Specular : SV_TARGET3;
};

PS_OUT PS_Main(GS_OUT input) : SV_TARGET {
	PS_OUT output;
	
	output.Color    = DiffuseMap.Sample(samplerState, input.TexCoord);
	output.PosWorld	= input.PosW;
	output.Normal   = float4(input.NormalW, 1.0f);
	output.Specular = float4(ReflectiveColor.xyz, Gloss);
	
	return output;
}