//***************************************************************************************
// TreeSprite.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Uses the geometry shader to expand a point sprite into a y-axis aligned 
// billboard that faces the camera.
//***************************************************************************************

#include "LightHelper.fx"
 
cbuffer cbPerFrame
{	
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gViewProj;
};

SamplerState samLinear
{
	Filter   = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	row_major float4x4 World : WORLD;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float4 Color   : COLOR;
};

struct GeoOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
    float2 Tex     : TEXCOORD;
    uint   PrimID  : SV_PrimitiveID;
};



VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), vin.World);
	vout.PosH = mul(PosW, gViewProj);
	vout.Color = vin.Color;

	return vout;
}

void GetMoveToolAxes(float3 posW, out float4 posH, out float3 axes[3])
{
	posH = mul(float4(posW, 1.0f), gViewProj);
	//세계공간에서 각 축의 끝점을 구하고 투영공간으로 변환
	float4 xAxisEndPosH = mul(
		float4((posW + float3(1.0f, 0.0f, 0.0f)), 1.0f), gViewProj);
	float4 yAxisEndPosH = mul(
		float4((posW + float3(0.0f, 1.0f, 0.0f)), 1.0f), gViewProj);
	float4 zAxisEndPosH = mul(
		float4((posW + float3(0.0f, 0.0f, 1.0f)), 1.0f), gViewProj);

	//ndc 공간으로 변환
	float4 xAxisEndPosNDC = xAxisEndPosH / xAxisEndPosH.w;
	float4 yAxisEndPosNDC = yAxisEndPosH / yAxisEndPosH.w;
	float4 zAxisEndPosNDC = zAxisEndPosH / zAxisEndPosH.w;

	//XMVECTOR AxisDir[3];
}
 
 
[maxvertexcount(15)]
void GS(point VertexOut gin[1], 
        uint primID : SV_PrimitiveID, 
        inout TriangleStream<GeoOut> triStream)
{	
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//

	//float3 up = float3(0.0f, 1.0f, 0.0f);
	//float3 look = gEyePosW - gin[0].CenterW;
	//look.y = 0.0f; // y-axis aligned, so project to xz-plane
	//look = normalize(look);
	//float3 right = cross(up, look);

	////
	//// Compute triangle strip vertices (quad) in world space.
	////
	//float halfWidth  = 0.5f*gin[0].SizeW.x;
	//float halfHeight = 0.5f*gin[0].SizeW.y;
	//
	//float4 v[4];
	//v[0] = float4(gin[0].CenterW + halfWidth*right - halfHeight*up, 1.0f);
	//v[1] = float4(gin[0].CenterW + halfWidth*right + halfHeight*up, 1.0f);
	//v[2] = float4(gin[0].CenterW - halfWidth*right - halfHeight*up, 1.0f);
	//v[3] = float4(gin[0].CenterW - halfWidth*right + halfHeight*up, 1.0f);

	////
	//// Transform quad vertices to world space and output 
	//// them as a triangle strip.
	////
	//GeoOut gout;
	//[unroll]
	//for(int i = 0; i < 4; ++i)
	//{
	//	gout.PosH     = mul(v[i], gViewProj);
	//	gout.PosW     = v[i].xyz;
	//	gout.NormalW  = look;
	//	gout.PrimID   = primID;
	//	
	//	triStream.Append(gout);
	//}
	GeoOut gout;
	gout.PosH = float4(1.0f, 1.0f, 1.0f, 1.0f);
	gout.PosW = float3(1.0f, 1.0f, 1.0f);
	gout.NormalW = float3(1.0f, 1.0f, 1.0f);
	gout.Tex = float2(1.0f, 1.0f);
	gout.PrimID = primID;
	triStream.Append(gout);
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}

//---------------------------------------------------------------------------------------
// Techniques--just define the ones our demo needs; you can define the other 
//   variations as needed.
//---------------------------------------------------------------------------------------
technique11 BasicTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		//SetGeometryShader( CompileShader( gs_5_0, GS() ) );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}

