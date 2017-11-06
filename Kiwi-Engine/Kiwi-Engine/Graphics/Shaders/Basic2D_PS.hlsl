#pragma pack_matrix(row_major) //shader will convert matrices into row major automatically, no need to transpose them first

Texture2D shaderTexture; //texture resource to be used
SamplerState sampleType; //sampler to apply effects to the texture resource

cbuffer MaterialSettings : register(b0)
{
	float4 diffuseColor;
	float hasDiffuse; //set to 1.0 if the material has a diffuse texture
	float diffuseIsRenderTexture; //set to 1.0 if the diffuse texture is a render target
	float2 _ms_padding_;
}

cbuffer PerObjectProperties : register(b1)
{
	float renderingText; //set to 1.0 if rendering 2D text
	float textSharpening;
	float usePerVertexColor; //if set to 1.0, the shader will use the per-vertex color as the diffuse color, otherwise it will use the above diffuseColor
	float _po_padding_;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

float4 main( PixelInputType input ) : SV_TARGET
{

	float4 outputColor;

	//outputColor = mul( diffuseColor, 1.0 - hasDiffuse ); //if no texture, use diffuse color

	outputColor = mul( diffuseColor, 1.0 - hasDiffuse ) + mul( shaderTexture.Sample( sampleType, input.tex ), hasDiffuse ); //if no texture, use diffuse color, otherwise use texture color

	if( renderingText == 1.0f )
	{
		clip( outputColor.a - textSharpening ); //sharpen text by discarding any pixel with alpha less than specified value

		float4 textColor = mul(input.color, usePerVertexColor) + mul( diffuseColor, 1.0f - usePerVertexColor ); //set text color to either the per vertex color or the diffuse color

		outputColor.a -= 1.0f - textColor.a; //reduce texture transparency by the inverse of the diffuse color alpha

		clip( outputColor.a ); //alpha may be less than 0 now, clip any such pixels

		/*only keep texture alpha value, use desired text color instead*/
		outputColor.r = textColor.r;
		outputColor.g = textColor.g;
		outputColor.b = textColor.b;
	}

	outputColor = saturate( outputColor );

	return outputColor;
}