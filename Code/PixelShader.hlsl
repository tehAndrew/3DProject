Texture2D tex      : register(t0);
SamplerState ssTex : register(s0);

cbuffer Light : register(b0) {
	float4 AmbientColor;
	float4 DiffuseColor;
	float4 SpecularColor;
	float4 LightPos;
	float  SpecPow;
};

cbuffer Camera : register(b1) {
	float4 CameraPos;
};

struct GS_OUT {
	float4 PosWVP   : SV_POSITION;
	float4 PosW     : POSITION;
	float3 NormalW  : NORMAL;
	float2 TexCoord : TEXCOORD;
};

float4 PS_Main(GS_OUT input) : SV_Target {
	
	float4 finalColor;
	float4 textureColor;
	float4 lightDist;
	float3 lightDir;
	float3 viewDir;
	float  lightIntensity;
	float3 reflection;
	float4 specular;

	// Texture color sampling
	textureColor = tex.Sample(ssTex, input.TexCoord);

	// Direction vector calculations
	lightDir  = normalize(LightPos.xyz - input.PosW.xyz);
	viewDir   = normalize(CameraPos.xyz - input.PosW.xyz);
	
	// Calculate light intensity
	lightIntensity = max(dot(input.NormalW, lightDir), 0.0f);

	// Apply Ambient lighting
	finalColor = AmbientColor;

	specular = float4(0.f, 0.f, 0.f, 0.f);
	
	if (lightIntensity > 0.0f) {
		// Calculate Specular highlight
		reflection = normalize(2 * lightIntensity * input.NormalW - lightDir);
		specular = pow(saturate(dot(reflection, viewDir)), SpecPow) * SpecularColor;

		// Apply Diffuse lighting
		finalColor = min(finalColor + (DiffuseColor * lightIntensity), 1.f);
	}

	// Apply Pixel Color
	finalColor *= textureColor;

	// Apply Specular lighting
	finalColor = saturate(finalColor + specular);
	finalColor = min(finalColor, 1.0f);

	return finalColor;
}