#define _NormalMap
#define _DiffuseMap
#define _SpecularMap

#include "MaterialTemplate.hlsl"

cbuffer PerObject
{
#ifdef _Skinning
	#define MaxNumBone 92
	float4x4 SkinMatrices[MaxNumBone];
#endif	
	float4x4 WorldView;
};

float4x4 Projection;

struct VSInput 
{
	float3 Pos 		 	 : POSITION;
#ifdef _Skinning
	float4 BlendWeights  : BLENDWEIGHTS;
	uint4 iBlendIndices  :  BLENDINDICES;
#endif
	float3 Normal		 : NORMAL;
	float2 Tex			 : TEXCOORD0;
#ifdef _NormalMap
	float3 Tangent		 : TANGENT;
	float3 Binormal      : BINORMAL;
#endif
};

struct VSOutput
{
	float4 PosHS : SV_POSITION;
	float4 PosVS : TEXCOORD0;
	float2 Tex   : TEXCOORD1;

#ifdef _NormalMap
	float3x3 TangentToView : TEXCOORD3;
#else
	float3 NormalVS 	   : TEXCOORD3;
#endif 
};

//---------------------------------------------------------------
VSOutput VSMain(VSInput input)
{
	VSOutput output = (VSOutput)0;

#ifdef _Skinning
	float4x4 SkinMatrix = SkinMatrices[input.BlendIndices[0]] * input.BlendWeights[0] +
					      SkinMatrices[input.BlendIndices[1]] * input.BlendWeights[1] +
					      SkinMatrices[input.BlendIndices[2]] * input.BlendWeights[2] +
					      SkinMatrices[input.BlendIndices[3]] * input.BlendWeights[3];
#endif
	
	// calculate position in view space:
#ifdef _Skinning
	mat4 SkinWorldView = SkinMatrix * WorldView;
	output.PosVS = mul( float4(input.Pos, 1.0), SkinWorldView );
#else
	output.PosVS = mul( float4(input.Pos, 1.0), WorldView );
#endif

	// calculate view space normal.
#ifdef _Skinning
	float3 normal = normalize( mul(input.Normal, (float3x3)SkinWorldView) );
#else
	float3 normal = normalize( mul(input.Normal, (float3x3)WorldView) );
#endif

	// calculate tangent and binormal.
#ifdef _NormalMap
	#ifdef _Skinning
		float3 tangent = normalize( mul(input.Tangent, (float3x3)SkinWorldView) );
		float3 binormal = normalize( mul(input.Binormal, (float3x3)SkinWorldView) );
	#else
		float3 tangent = normalize( mul(input.Tangent, (float3x3)WorldView) );
		float3 binormal = normalize( mul(input.Binormal, (float3x3)WorldView) );
	#endif

	// actualy this is a world to tangent matrix, because we always use V * Mat.
	output.TangentToView = float3x3( tangent, binormal, normal);

	// transpose to get tangent to world matrix
	// oTangentToView = transpose(oTangentToView);
#else
	output.NormalVS = normal;
#endif
		
	output.PosHS = mul(output.PosVS, Projection);

	return output;
}

//---------------------------------------------------------------
 float Luminance(in float3 color)
 {
	return dot(color, float3(0.2126, 0.7152, 0.0722));
 }

void GBufferPSMain(in VSOutput input,
                   out float4 oFragColor0 : SV_Target0,
				   out float4 oFragColor1 : SV_Target1 )
{

#ifdef _DiffuseMap
	float4 diffuseTap = DiffuseMap.Sample(LinearSampler, input.Tex);
	float3 albedo = diffuseTap.rgb;
	#ifdef _AlphaTest
		if( diffuseTap.a < 0.01 ) discard;
	#endif	
#else
	float3 albedo = DiffuseColor;
#endif

	// specular material
#ifdef _SpecularMap
	float4 specularTap = SpecularMap.Sample(LinearSampler, input.Tex);
	//vec3 specular = specularTap.rgb;
	float3 specular = specularTap.rrr;
	#ifdef _GlossyMap
		float shininess = specularTap.a;
	#else 
		float shininess = Shininess / 255.0;
	#endif
#else
	float3 specular = SpecularColor;
	float shininess = Shininess / 255.0;
#endif
	
	// normal map
#ifdef _NormalMap
	float3 normal = NormalMap.Sample(LinearSampler , input.Tex ).rgb * 2.0 - 1.0;
	normal = normalize( mul(normal, input.TangentToView) );
#else
	float3 normal = normalize(input.NormalVS);
#endif	
	
	normal = normal * 0.5 + 0.5;
	//CompressUnsignedNormalToNormalsBuffer(normal);	
	
	oFragColor0 = float4(normal.xyz, shininess);
	oFragColor1 = float4(albedo.rgb, Luminance(specular));	 // Specular luminance
}
