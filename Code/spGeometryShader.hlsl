struct VS_OUT {
	float4 Pos : POSITION;
};

struct GS_OUT {
	float4 Pos        : SV_POSITION;
	uint   ArrayIndex : SV_RenderTargetArrayIndex;
};

cbuffer Matrices : register(b0) {
	float4x4 viewMatrix;
	float4x4 projMatrix;
}

[maxvertexcount(18)]
void GS_Main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> outputStream) {
	for (int f = 0; f < 6; ++f) {
		GS_OUT output;
		output.ArrayIndex = f;

		for (int v = 0; v < 3; v++) {
			output.Pos = mul(input[v].Pos, viewMatrix);
			output.Pos = mul(output.Pos, projMatrix);
		}

		outputStream.Append(output);
	}
	outputStream.RestartStrip();
}