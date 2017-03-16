Texture2D DiffuseTex  : register(t0);
Texture2D PosTex      : register(t1);
Texture2D NormalTex   : register(t2);
Texture2D SpecularTex : register(t3);
TextureCube ShadowMap : register(t4);

SamplerState SampleState       : register(s0);

cbuffer Light : register(b0) {
	float4 AmbientColor;  // The ambient light of the scene where the light is. Probably better to put 
						  // this as a material if more lights are present.
	float4 DiffuseColor; // The color of the light.
	float4 LightPos;
	float  AttenuationFactor;
};

cbuffer Camera : register(b1) {
	float4 CameraPos;
};

cbuffer Frustum : register(b2) {
	float FarPlane;
	float Pad1, Pad2, Pad3; // Paddings
};

struct VS_OUT {
	float4 Pos      : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 PS_Main(VS_OUT input) : SV_Target {
	float4 diffuseSample  = DiffuseTex.Sample(SampleState, input.TexCoord);
	float4 posSample      = PosTex.Sample(SampleState, input.TexCoord);
	float4 normalSample   = NormalTex.Sample(SampleState, input.TexCoord);
	float4 specularSample = float4(SpecularTex.Sample(SampleState, input.TexCoord).xyz, 1.f);
	float  glossSample    = SpecularTex.Sample(SampleState, input.TexCoord).w;
	
	float  distToLight    = length(LightPos.xyz - posSample.xyz);
	float3 lightDir       = normalize(LightPos.xyz - posSample.xyz);
	float3 viewDir        = normalize(CameraPos.xyz - posSample.xyz);
	float  lightIntensity = saturate(dot(normalSample.xyz, lightDir));
	
	float4 ambient  = diffuseSample * AmbientColor;
	float4 diffuse  = diffuseSample * DiffuseColor * lightIntensity;
	float4 specular = float4(0.f, 0.f, 0.f, 1.f);

	if (lightIntensity > 0.f && glossSample > 0.f) {
		float3 reflection = normalize(2 * lightIntensity * normalSample.xyz - lightDir);
		specular = pow(saturate(dot(reflection, viewDir)), glossSample) * (specularSample * DiffuseColor);
	}

	// Shadow calculations
	float shortestDistToLight = ShadowMap.Sample(SampleState, posSample.xyz - LightPos.xyz).r;
	shortestDistToLight *= FarPlane;
	float shadowBias = 0.5f;
	float shadow = 1.f; // There is shadow

	if (distToLight - shadowBias > shortestDistToLight) {
		shadow = 0.f;
	}

	float  attenuation = (1.f / (1.f + AttenuationFactor * pow(distToLight, 2)));
	float4 finalColor = saturate(ambient + shadow * attenuation * (diffuse + specular));
	return finalColor;
}