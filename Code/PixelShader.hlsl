struct GS_OUT {
	float4 Pos       : SV_POSITION;
	float4 WorldPos  : POSITION;
	float3 Normal    : NORMAL;
	float3 Color     : COLOR;
};

float4 PS_Main(GS_OUT input) : SV_Target {
	float3 lightDir;
	float lightIntensity;

	lightDir = float3(0.0f, 10.0f, -2.0f) - input.WorldPos.xyz;
	lightDir = normalize(lightDir);
	lightIntensity = max(dot(input.Normal, lightDir), 0.0f);

	float3 finalColor = input.Color * lightIntensity;

	return float4(finalColor, 1.0f);
}