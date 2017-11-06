#pragma pack_matrix(row_major) //shader will convert matrices into row major automatically, no need to transpose them first

Texture2D shaderTexture; //texture resource to be used
SamplerState sampleType; //sampler to apply effects to the texture resource

cbuffer TextProperties : register(b0)
{
	float4 properties;
}

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

float4 main( PixelInputType input ) : SV_TARGET
{
	float4 textureColor;
	float4 difColor = input.color;

	//sample pixel color using the sampler
	textureColor = shaderTexture.Sample( sampleType, input.tex );

	textureColor.a -= 1.0f - difColor.a; //reduce texture transparency by the inverse of the diffuse color alpha

	clip( textureColor.a - properties.x ); //sharpen text by discarding any pixel with alpha less than specified value

	/*only keep texture alpha value, use diffuse color instead*/
	textureColor.r = difColor.r;
	textureColor.g = difColor.g;
	textureColor.b = difColor.b;

	return textureColor;
}