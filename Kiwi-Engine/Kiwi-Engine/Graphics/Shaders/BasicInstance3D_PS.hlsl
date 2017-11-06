#pragma pack_matrix(row_major) //shader will convert matrices into row major automatically, no need to transpose them first

Texture2D textures[4];
SamplerState textureSampler;

cbuffer MaterialBuffer : register(b0)
{
	float4 diffuseColor;
	float4 isTextured;
};

cbuffer FrameBuffer : register(b1)
{
	float4 ambientLight;
	float4 diffuseLight;
	float4 lightDirection;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float3 tangent : TANGENT;
	float3 bitangent : BINORMAL;
};

float4 main( VertexOutput input ) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	//The light intensity value is calculated as the dot product between the normal vector of triangle and 
	//the light direction vector
	lightDir = (float3) - lightDirection;

	if( isTextured.x == 1.0f )
	{
		//calculate the amount of light on this pixel
		if( isTextured.y == 1.0f )
		{
			//using bump map
			float4 bumpMap;
			float3 bumpNormal;

			//sample the pixel in the bump map
			bumpMap = textures[1].Sample( textureSampler, input.texCoord );

			//expand the range of the normal value from (0,1) to (-1,1)
			bumpMap = (bumpMap * 2.0f) - 1.0f;

			//calculate the bump normal from the data in the bump map
			bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.bitangent) + (bumpMap.z * input.normal);
			bumpNormal = normalize( bumpNormal );

			lightIntensity = saturate( dot( bumpNormal, lightDir ) );

		} else
		{
			//no bump map
			lightIntensity = saturate( dot( input.normal, lightDir ) );
		}

		//combine color and texture color to get final color
		textureColor = textures[0].Sample( textureSampler, input.texCoord );

		color = saturate( diffuseLight * lightIntensity );
		color = color * textureColor;

	} else
	{
		//set the ambient color for all pixels first
		color = ambientLight;

		//calculate the amount of light on this pixel
		lightIntensity = saturate( dot( input.normal, lightDir ) );

		//Check if the N dot L is greater than zero. 
		//If it is then add the diffuse color to the ambient color. 
		if( lightIntensity > 0.0f )
		{
			color += (diffuseLight * lightIntensity);
		}

		//combine diffuse color with diffuse light
		color = saturate( color );

		//combine color and diffuse color to get the final color
		color = color * diffuseColor;
		color.a = diffuseColor.a;
	}

	return color;

}