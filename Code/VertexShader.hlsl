struct VS_IN {
	float3 Pos   : POSITION;
	float3 Color : COLOR;
};

struct VS_OUT {
	float4 Pos   : POSITION;
	float3 Color : COLOR;
};

VS_OUT VS_Main(VS_IN input) {
	VS_OUT output;
	output.Pos   = float4(input.Pos, 1.0f);
	output.Color = input.Color;

	return output;
}