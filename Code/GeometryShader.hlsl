struct VS_OUT {
	float4 Pos   : POSITION;
	float3 Color : COLOR;
};

struct GS_OUT {
	float4 Pos      : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 Normal   : NORMAL;
	float3 Color    : COLOR;
};

cbuffer Matrices : register(b0) {
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
}

[maxvertexcount(3)]
void GS_Main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> outputStream) {
	GS_OUT output[3];

	float3 normal = cross(input[1].Pos.xyz - input[0].Pos.xyz, input[2].Pos.xyz - input[0].Pos.xyz);
	normal = mul(normal, (float3x3) worldMatrix);
	normal = normalize(normal);

	for (uint i = 0; i < 3; i++) {
		output[i].Normal = normal;
		output[i].Color = input[i].Color;
		output[i].Pos = input[i].Pos;

		output[i].Pos = mul(output[i].Pos, worldMatrix);
		output[i].WorldPos = output[i].Pos;
		output[i].Pos = mul(output[i].Pos, viewMatrix);
		output[i].Pos = mul(output[i].Pos, projMatrix);

		outputStream.Append(output[i]);
	}
}