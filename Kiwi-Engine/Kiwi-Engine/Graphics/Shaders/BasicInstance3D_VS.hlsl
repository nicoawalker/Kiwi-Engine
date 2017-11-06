#pragma pack_matrix(row_major) //shader will convert matrices into row major automatically, no need to transpose them first

cbuffer ObjectBuffer : register(b0) //contains data modified for every entity
{
	float4x4 viewMat;
	float4x4 projMat;
};

//struct VertexInput
//{
//	float4 position : POSITION;
//	float2 texCoord : TEXCOORD;
//	float3 normal : NORMAL;
//	float4 color : COLOR;
//	float3 tangent : TANGENT;
//	float3 bitangent : BITANGENT;
//
//	float4x4 instWorldMat : INSTANCEWORLD0; //world matrix for the instance
//};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal: NORMAL;
	float4 color : COLOR;
	float3 tangent : TANGENT;
	float3 bitangent : BINORMAL;
};


VertexOutput main( float4 position : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL, float4 color : COLOR,
				   float3 tangent : TANGENT, float3 bitangent : BITANGENT, float4 world0 : INSTANCEWORLD0,
				   float4 world1 : INSTANCEWORLD1, float4 world2 : INSTANCEWORLD2, float4 world3 : INSTANCEWORLD3 )
//VertexOutput main( VertexInput input )
{
	VertexOutput output;

	//float4x4 instWorldMat = float4x4( world0, world1, world2, world3 );

	float4x4 instWorldMat = transpose( float4x4(world0.x, world1.x, world2.x, world3.x,
												 world0.y, world1.y, world2.y, world3.y,
												 world0.z, world1.z, world2.z, world3.z,
												 world0.w, world1.w, world2.w, world3.w) );

	/*create wvp matrix for the instance*/
	//float4x4 wvp = mul( instWorldMat, mul( viewMat, projMat ) );

	position.w = 1.0f;

	output.position = mul( position, instWorldMat );
	output.position = mul( output.position, viewMat );
	output.position = mul( output.position, projMat );

	output.texCoord = texCoord;

	output.normal = mul(normal, (float3x3)instWorldMat );
	output.color = color;

	output.tangent = mul( tangent, (float3x3)instWorldMat );
	output.tangent = normalize( output.tangent );

	output.bitangent = mul( bitangent, (float3x3)instWorldMat );
	output.bitangent = normalize( output.bitangent );

	return output;
}