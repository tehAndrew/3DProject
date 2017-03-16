struct PS_IN {
	float4 wPos : POSITION;
	float4 Pos  : SV_POSITION;
};

cbuffer Light : register(b0) {
	float4 AmbientColor;      // Unused here
	float4 DiffuseColor;      // Unused here
	float4 LightPos;
	float  AttenuationFactor; // Unused here
};

cbuffer Frustum : register(b1) {
	float FarPlane;
	float Pad1, Pad2, Pad3; // Paddings
};

float PS_Main(PS_IN input) : SV_DEPTH {
	float lightDist = length(input.wPos.xyz - LightPos.xyz);
	
	return (lightDist / FarPlane);
}