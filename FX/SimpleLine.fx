//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"


cbuffer cbPerFrame
{
	DirectionalLight gDirLights[DIRECTIONALLIGHT_SIZE];
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gViewProj;
};


SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};
 
struct VertexIn
{
	float3 PosW    : POSITION;
	float4 Color : COLOR;
};



struct VertexOut
{
	float4 PosH       : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	

	//바운딩 박스 렌더링에선 gWorld가 세계행렬, 옥트리 렌더링시 gWorld가 항등행렬
	float4 worldTemp = mul(float4(vin.PosW, 1.0f), gWorld);
	vout.PosH = mul(worldTemp, gViewProj);
	vout.Color = vin.Color;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}

technique11 Light1
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ));
    }
}