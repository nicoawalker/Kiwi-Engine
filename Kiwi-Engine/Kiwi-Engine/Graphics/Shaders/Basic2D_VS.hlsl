#pragma pack_matrix(row_major) //shader will convert matrices into row major automatically, no need to transpose them first

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

cbuffer ObjectBuffer : register(b0) //contains data modified for every renderable
{
	matrix wvp;
};

PixelInputType main( VertexInputType input )
{

	PixelInputType output;

	input.position.w = 1.0f;
	input.position.z = 0.0f;

	output.position = mul( input.position, wvp );

	// Store the input texture for the pixel shader to use.
	output.tex = input.tex;

	output.normal = input.normal;
	output.color = input.color;

	return output;
}