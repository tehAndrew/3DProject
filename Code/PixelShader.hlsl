struct GS_OUT {
	float4 Pos       : SV_POSITION;
	float4 WorldPos  : POSITION;
	float3 Normal    : NORMAL;
	float3 Color     : COLOR;
};

float4 PS_Main(GS_OUT input) : SV_Target {
	float3 lightDir;
	float lightIntensity;

	lightDir = -((float3)input.WorldPos - float3(0.0f, 0.0f, -2.0f));
	lightDir = normalize(lightDir);
	lightIntensity = max(dot(input.Normal, lightDir), 0.0f);

	float4 finalColor = float4(input.Color, 1.0f) * lightIntensity;

	return finalColor;
}