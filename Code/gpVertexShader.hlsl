struct VS_IN {
	float3 PosL      : POSITION;
	float3 NormalL   : NORMAL;
	float2 TexCoord  : TEXCOORD;
};

struct VS_OUT {
	float4 PosL      : POSITION;
	float3 NormalL   : NORMAL;
	float2 TexCoord  : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input) {
	VS_OUT output;
	output.PosL      = float4(input.PosL, 1.0f);
	output.NormalL   = input.NormalL;
	output.TexCoord  = input.TexCoord;

	return output;
}