//=============================================================================
// Blur.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs a separable blur with a blur radius of 5.  
//=============================================================================

#define BONESIZE 160
cbuffer cbSkinned
{
	float4x4 gBoneTransforms[BONESIZE];
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

cbuffer cbPerInstance
{
	uint instanceID;
};

StructuredBuffer<vertex> gVertices;
StructuredBuffer<skinData> gInputSkinData;

RWStructuredBuffer<resultVertex> gDestVertices;

#define N 256
//#define CacheSize (N + 2*gBlurRadius)

groupshared float4x4 gBoneCache[BONESIZE];

[numthreads(N, 1, 1)]
void Skinning(int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupThreadID : SV_GroupThreadID)
{
	//모든 뼈 최종행렬을 공유메모리에 저장
	if (groupThreadID.x < BONESIZE)
		gBoneCache[groupThreadID.x] = gBoneTransforms[groupThreadID.x];
	
	//동기화
	GroupMemoryBarrierWithGroupSync();

	//해당 메쉬의 정점 개수를 구함
	static uint vertexSrcSize = 0;
	static uint bufferStride = 0;
	gVertices.GetDimensions(vertexSrcSize, bufferStride);
	
	uint vertexStart = vertexSrcSize * instanceID;

	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = gInputSkinData[dispatchThreadID.x].Weights.x;
	weights[1] = gInputSkinData[dispatchThreadID.x].Weights.y;
	weights[2] = gInputSkinData[dispatchThreadID.x].Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	float3 bitanL = float3(0.0f, 0.0f, 0.0f);

	int boneIdx[4];
	boneIdx[0] = gInputSkinData[dispatchThreadID.x].BoneIndices[0];
	boneIdx[1] = gInputSkinData[dispatchThreadID.x].BoneIndices[1];
	boneIdx[2] = gInputSkinData[dispatchThreadID.x].BoneIndices[2];
	boneIdx[3] = gInputSkinData[dispatchThreadID.x].BoneIndices[3];
	
	for (int i = 0; i < 4; ++i)
	{
		// Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
		if (gInputSkinData[dispatchThreadID.x].BoneIndices[i] != -1)
		{
			
			posL += weights[i] * mul(float4(gVertices[dispatchThreadID.x].PosL, 1.0f),
				gBoneCache[boneIdx[i]]).xyz;
			normalL += weights[i] * mul(gVertices[dispatchThreadID.x].NormalL,
				(float3x3)gBoneCache[boneIdx[i]]);
			tangentL += weights[i] * mul(gVertices[dispatchThreadID.x].tan,
				(float3x3)gBoneCache[boneIdx[i]]);
			bitanL += weights[i] * mul(gVertices[dispatchThreadID.x].bitan,
				(float3x3)gBoneCache[boneIdx[i]]);
		}
		else
		{
			posL += weights[i] * mul(float4(gVertices[dispatchThreadID.x].PosL, 1.0f),
				gBoneCache[boneIdx[0]]).xyz;
			normalL += weights[i] * mul(gVertices[dispatchThreadID.x].NormalL,
				(float3x3)gBoneCache[boneIdx[0]]);
			tangentL += weights[i] * mul(gVertices[dispatchThreadID.x].tan,
				(float3x3)gBoneCache[boneIdx[0]]);
			bitanL += weights[i] * mul(gVertices[dispatchThreadID.x].bitan,
				(float3x3)gBoneCache[boneIdx[0]]);
		}
	}

	gDestVertices[vertexStart + dispatchThreadID.x].PosL = posL;
	gDestVertices[vertexStart + dispatchThreadID.x].PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	gDestVertices[vertexStart + dispatchThreadID.x].PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	gDestVertices[vertexStart + dispatchThreadID.x].NormalW = mul(normalL, (float3x3)gWorldInvTranspose);
	gDestVertices[vertexStart + dispatchThreadID.x].Tex = gVertices[dispatchThreadID.x].Tex;
	gDestVertices[vertexStart + dispatchThreadID.x].TanW = float4(mul(tangentL, (float3x3)gWorld), 0.0f);
	gDestVertices[vertexStart + dispatchThreadID.x].BiTanW = float4(mul(bitanL, (float3x3)gWorld), 0.0f);

}

[numthreads(N, 1, 1)]
void SkinningOrigin(int3 dispatchThreadID : SV_DispatchThreadID)
{
	// Init array or else we get strange warnings about SV_POSITION.
	uint len = 0;
	uint bufferStride = 0;
	gVertices.GetDimensions(len, bufferStride);


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
