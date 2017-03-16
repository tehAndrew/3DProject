struct VS_IN {
	float3 Pos       : POSITION;
	float2 TexCoord  : TEXCOORD;
};

struct VS_OUT {
	float4 Pos       : SV_POSITION;
	float2 TexCoord  : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input) {
	VS_OUT output;

	output.Pos = float4(input.Pos, 1.f);
	output.TexCoord = input.TexCoord;

	return output;
}