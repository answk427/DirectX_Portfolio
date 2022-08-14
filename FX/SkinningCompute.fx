//=============================================================================
// Blur.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs a separable blur with a blur radius of 5.  
//=============================================================================

cbuffer cbSkinned
{
	float4x4 gBoneTransforms[128];
};

struct vertex
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
	float3 tan     : TANGENT;
	float3 bitan   : BITANGENT;
};

struct resultVertex
{
	float3 PosL;
	float4 PosH;
	float3 PosW;
	float3 NormalW;
	float2 Tex;
	float4 TanW;
	float4 BiTanW;
};		

struct skinData
{
	float3 Weights    : WEIGHTS;
	int4 BoneIndices : BONEINDICES;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
};

StructuredBuffer<vertex> gVertices;
StructuredBuffer<skinData> gInputSkinData;

RWStructuredBuffer<resultVertex> gDestVertices;

#define N 256
#define CacheSize (N + 2*gBlurRadius)

/*[numthreads(N, 1, 1)]
void Skinning(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = gInputSkinData[dispatchThreadID.x].Weights.x;
	weights[1] = gInputSkinData[dispatchThreadID.x].Weights.y;
	weights[2] = gInputSkinData[dispatchThreadID.x].Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	//float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		// Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
		if (gInputSkinData[dispatchThreadID.x].BoneIndices[i] != -1)
		{
			posL += weights[i] * mul(float4(gVertices[dispatchThreadID.x].PosL, 1.0f),
				gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[i]]).xyz;
			normalL += weights[i] * mul(gVertices[dispatchThreadID.x].NormalL, 
				(float3x3)gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[i]]);
			//tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
		}
		else
			posL += weights[i] * mul(float4(gVertices[dispatchThreadID.x].PosL, 1.0f),
				gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[0]]).xyz;
	}

	gVertices[dispatchThreadID.x].PosL = posL;


	// Transform to world space space.
	//vout.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	//vout.NormalW = mul(normalL, (float3x3)gWorldInvTranspose);
	//vout.TangentW = float4(mul(tangentL, (float3x3)gWorld), vin.TangentL.w);

	// Transform to homogeneous clip space.
	//vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle.
	//vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Generate projective tex-coords to project shadow map onto scene.
	//vout.ShadowPosH = mul(float4(posL, 1.0f), gShadowTransform);

	// Generate projective tex-coords to project SSAO map onto scene.
	//vout.SsaoPosH = mul(float4(posL, 1.0f), gWorldViewProjTex);
}*/

[numthreads(N, 1, 1)]
void Skinning(int3 dispatchThreadID : SV_DispatchThreadID)
{
	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = gInputSkinData[dispatchThreadID.x].Weights.x;
	weights[1] = gInputSkinData[dispatchThreadID.x].Weights.y;
	weights[2] = gInputSkinData[dispatchThreadID.x].Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	float3 bitanL = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		// Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
		if (gInputSkinData[dispatchThreadID.x].BoneIndices[i] != -1)
		{
			posL += weights[i] * mul(float4(gVertices[dispatchThreadID.x].PosL, 1.0f),
				gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[i]]).xyz;
			normalL += weights[i] * mul(gVertices[dispatchThreadID.x].NormalL,
				(float3x3)gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[i]]);
			tangentL += weights[i] * mul(gVertices[dispatchThreadID.x].tan,
				(float3x3)gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[i]]);
			bitanL += weights[i] * mul(gVertices[dispatchThreadID.x].bitan,
				(float3x3)gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[i]]);
		}
		else
		{
			posL += weights[i] * mul(float4(gVertices[dispatchThreadID.x].PosL, 1.0f),
				gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[0]]).xyz;
			normalL += weights[i] * mul(gVertices[dispatchThreadID.x].NormalL,
				(float3x3)gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[0]]);
			tangentL += weights[i] * mul(gVertices[dispatchThreadID.x].tan,
				(float3x3)gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[0]]);
			bitanL += weights[i] * mul(gVertices[dispatchThreadID.x].bitan,
				(float3x3)gBoneTransforms[gInputSkinData[dispatchThreadID.x].BoneIndices[0]]);
		}
			
	}
	
	gDestVertices[dispatchThreadID.x].PosL = posL;
	gDestVertices[dispatchThreadID.x].PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	gDestVertices[dispatchThreadID.x].PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	gDestVertices[dispatchThreadID.x].NormalW = mul(normalL, (float3x3)gWorldInvTranspose);
	gDestVertices[dispatchThreadID.x].Tex = gVertices[dispatchThreadID.x].Tex;
	gDestVertices[dispatchThreadID.x].TanW = float4(mul(tangentL, (float3x3)gWorld), 0.0f);
	gDestVertices[dispatchThreadID.x].BiTanW = float4(mul(bitanL, (float3x3)gWorld), 0.0f);

}

technique11 ComputeSkinning
{
    pass P0
    {
		SetVertexShader( NULL );
        SetPixelShader( NULL );
		SetComputeShader( CompileShader( cs_5_0, Skinning() ) );
    }
}
