struct VS_IN {
	float3 Pos      : POSITION;
	float3 Normal   : NORMAL;   // UNUSED
	float2 TexCoord : TEXCOORD; // UNUSED
};

struct PS_IN {
	float4 wPos : POSITION;
	float4 Pos  : SV_POSITION;
};

cbuffer Matrices : register(b0) {
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

PS_IN VS_Main(VS_IN input) {
	PS_IN output;
	output.Pos  = float4(input.Pos, 1.f);
	output.Pos  = mul(output.Pos, worldMatrix);
	output.wPos = output.Pos;
	output.Pos  = mul(output.Pos, viewMatrix);
	output.Pos  = mul(output.Pos, projMatrix);
	
	return output;
}