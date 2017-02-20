struct VS_OUT {
	float4 PosL      : POSITION;
	float3 NormalL   : NORMAL;
	float2 TexCoord  : TEXCOORD;
};

struct GS_OUT {
	float4 PosWVP   : SV_POSITION;
	float4 PosW     : POSITION;
	float3 NormalW  : NORMAL;
	float2 TexCoord : TEXCOORD;
};

cbuffer Matrices : register(b0) {
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
}

[maxvertexcount(3)]
void GS_Main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> outputStream) {
	GS_OUT output[3];

	for (uint i = 0; i < 3; i++) {
		output[i].NormalW  = mul(input[i].NormalL, (float3x3) worldMatrix);
		output[i].NormalW  = normalize(output[i].NormalW);
		output[i].TexCoord = input[i].TexCoord;
		output[i].PosWVP   = input[i].PosL;

		output[i].PosWVP = mul(output[i].PosWVP, worldMatrix);
		output[i].PosW   = output[i].PosWVP;
		output[i].PosWVP = mul(output[i].PosWVP, viewMatrix);
		output[i].PosWVP = mul(output[i].PosWVP, projMatrix);

		outputStream.Append(output[i]);
	}
}