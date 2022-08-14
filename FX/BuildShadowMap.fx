//=============================================================================
// BuildShadowMap.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Effect used to build the shadow map.
//
// A lot of code is copy and pasted from DisplacementMap.fx.  When drawing 
// depth to shadow map, we need to tessellate the geometry the same way
// when rendering from the eye so that the shadow map records the same
// geometry the eye sees.
//=============================================================================

cbuffer cbPerFrame
{
	float3 gEyePosW;
	
	float gHeightScale;
	float gMaxTessDistance;
	float gMinTessDistance;
	float gMinTessFactor;
	float gMaxTessFactor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
}; 

cbuffer cbSkinned
{
	float4x4 gBoneTransforms[96];
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gHeightMap;
Texture2DArray gDiffuseMapArray;

SamplerState samHeightmap
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float2 Tex      : TEXCOORD;
};

struct SkinnedVertexIn
{
	float3 PosL       : POSITION;
	float3 NormalL    : NORMAL;
	float2 Tex        : TEXCOORD;
	float4 TangentL   : TANGENT;
	float3 Weights    : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

struct TerrainVertexIn
{
	float3 PosL     : POSITION;
	float2 Tex      : TEXCOORD0;
	float2 BoundsY  : TEXCOORD1;
};

struct TerrainVertexOut
{
	float3 PosW     : POSITION;
	float2 Tex      : TEXCOORD0;
	float2 BoundsY  : TEXCOORD1;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex  : TEXCOORD;
};

struct ComputedVertex
{
	float3 PosL;
	float4 PosH;
	float3 PosW;
	float3 NormalW;
	float2 Tex;
	float4 TanW;
	float4 BiTanW;
};

StructuredBuffer<ComputedVertex> gVertices;


float CalcTessFactor(float3 p)
{
	float d = distance(p, gEyePosW);

	// max norm in xz plane (useful to see detail levels from a bird's eye).
	//float d = max( abs(p.x-gEyePosW.x), abs(p.z-gEyePosW.z) );

	float s = saturate((d - gMinTessDistance) / (gMaxTessDistance - gMinTessDistance));

	return pow(2, (lerp(gMaxTessFactor, gMinTessFactor, s)));
}

VertexOut SkinningVS(uint vertexID : SV_VertexID)
{
	VertexOut vout;
	//vout.PosH = gVertices[vertexID].PosH;
	vout.PosH = mul(float4(gVertices[vertexID].PosL, 1.0f), mul(gWorld, gViewProj));
	vout.Tex = mul(float4(gVertices[vertexID].Tex, 0.0f, 1.0f), gTexTransform).xy;
	
	return vout;
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	//vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.PosH = mul(float4(vin.PosL, 1.0f), mul(gWorld, gViewProj));
	vout.Tex  = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}


VertexOut SkinnedVS(SkinnedVertexIn vin)
{
    VertexOut vout;

	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	weights[0] = vin.Weights.x;
	weights[1] = vin.Weights.y;
	weights[2] = vin.Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL     = float3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < 4; ++i)
	{
	    // Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
	    posL     += weights[i]*mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
	}
 
	// Transform to homogeneous clip space.
	//vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	vout.PosH = mul(float4(vin.PosL, 1.0f), mul(gWorld, gViewProj));

	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

TerrainVertexOut TeerrainVS(TerrainVertexIn vin)
{
	TerrainVertexOut vout;

	// Terrain specified directly in world space.
	vout.PosW = vin.PosL;

	// Displace the patch corners to world space.  This is to make 
	// the eye to patch distance calculation more accurate.
	vout.PosW.y = gHeightMap.SampleLevel(samHeightmap, vin.Tex, 0).r;
	
	// Output vertex attributes to next stage.
	vout.Tex = vin.Tex;
	vout.BoundsY = vin.BoundsY;

	return vout;
}

struct TessVertexOut
{
	float3 PosW       : POSITION;
	float3 NormalW    : NORMAL;
	float2 Tex        : TEXCOORD;
	float  TessFactor : TESS;
};

TessVertexOut TessVS(VertexIn vin)
{
	TessVertexOut vout;

	vout.PosW     = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.Tex      = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	float d = distance(vout.PosW, gEyePosW);

	// Normalized tessellation factor. 
	// The tessellation is 
	//   0 if d >= gMinTessDistance and
	//   1 if d <= gMaxTessDistance.  
	float tess = saturate( (gMinTessDistance - d) / (gMinTessDistance - gMaxTessDistance) );
	
	// Rescale [0,1] --> [gMinTessFactor, gMaxTessFactor].
	vout.TessFactor = gMinTessFactor + tess*(gMaxTessFactor-gMinTessFactor);

	return vout;
}

struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess  : SV_InsideTessFactor;
};

struct TerrainPatchTess
{
	float EdgeTess[4]   : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

TerrainPatchTess TerrainConstantHS(InputPatch<TerrainVertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
	TerrainPatchTess pt;

	////
	//// Frustum cull
	////

	
	// Do normal tessellation based on distance.
	//
		// It is important to do the tess factor calculation based on the
		// edge properties so that edges shared by more than one patch will
		// have the same tessellation factor.  Otherwise, gaps can appear.

		// Compute midpoint on edges, and patch center
		float3 e0 = 0.5f*(patch[0].PosW + patch[2].PosW);
		float3 e1 = 0.5f*(patch[0].PosW + patch[1].PosW);
		float3 e2 = 0.5f*(patch[1].PosW + patch[3].PosW);
		float3 e3 = 0.5f*(patch[2].PosW + patch[3].PosW);
		float3  c = 0.25f*(patch[0].PosW + patch[1].PosW + patch[2].PosW + patch[3].PosW);

	/*	pt.EdgeTess[0] = CalcTessFactor(e0);
		pt.EdgeTess[1] = CalcTessFactor(e1);
		pt.EdgeTess[2] = CalcTessFactor(e2);
		pt.EdgeTess[3] = CalcTessFactor(e3);

		pt.InsideTess[0] = CalcTessFactor(c);
		pt.InsideTess[1] = pt.InsideTess[0];*/

		pt.EdgeTess[0] = gMaxTessFactor;
		pt.EdgeTess[1] = gMaxTessFactor;
		pt.EdgeTess[2] = gMaxTessFactor;
		pt.EdgeTess[3] = gMaxTessFactor;

		pt.InsideTess[0] = gMaxTessFactor;
		pt.InsideTess[1] = pt.InsideTess[0];
		return pt;
	
}

PatchTess PatchHS(InputPatch<TessVertexOut,3> patch, 
                  uint patchID : SV_PrimitiveID)
{
	PatchTess pt;
	
	// Average tess factors along edges, and pick an edge tess factor for 
	// the interior tessellation.  It is important to do the tess factor
	// calculation based on the edge properties so that edges shared by 
	// more than one triangle will have the same tessellation factor.  
	// Otherwise, gaps can appear.
	pt.EdgeTess[0] = 0.5f*(patch[1].TessFactor + patch[2].TessFactor);
	pt.EdgeTess[1] = 0.5f*(patch[2].TessFactor + patch[0].TessFactor);
	pt.EdgeTess[2] = 0.5f*(patch[0].TessFactor + patch[1].TessFactor);
	pt.InsideTess  = pt.EdgeTess[0];
	
	return pt;
}

struct HullOut
{
	float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
	float2 Tex      : TEXCOORD;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HullOut HS(InputPatch<TessVertexOut,3> p, 
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
	HullOut hout;
	
	// Pass through shader.
	hout.PosW     = p[i].PosW;
	hout.NormalW  = p[i].NormalW;
	hout.Tex      = p[i].Tex;
	
	return hout;
}

struct TerrainHullOut
{
	float3 PosW     : POSITION;
	float2 Tex      : TEXCOORD0;
};

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("TerrainConstantHS")]
[maxtessfactor(64.0f)]
TerrainHullOut TerrainHS(InputPatch<TerrainVertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	TerrainHullOut hout;

	// Pass through shader.
	hout.PosW = p[i].PosW;
	hout.Tex = p[i].Tex;

	return hout;
}

struct TerrainDomainOut
{
	float4 PosH     : SV_POSITION;
	float3 PosW     : POSITION;
	float2 Tex      : TEXCOORD0;
	float2 TiledTex : TEXCOORD1;
};

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("quad")]
TerrainDomainOut TerrainDS(TerrainPatchTess patchTess,
	float2 uv : SV_DomainLocation,
	const OutputPatch<TerrainHullOut, 4> quad)
{
	TerrainDomainOut dout;

	// Bilinear interpolation.
	dout.PosW = lerp(
		lerp(quad[0].PosW, quad[1].PosW, uv.x),
		lerp(quad[2].PosW, quad[3].PosW, uv.x),
		uv.y);

	dout.Tex = lerp(
		lerp(quad[0].Tex, quad[1].Tex, uv.x),
		lerp(quad[2].Tex, quad[3].Tex, uv.x),
		uv.y);

	// Tile layer textures over terrain.
	//dout.TiledTex = dout.Tex*gTexScale; 
	dout.TiledTex = mul(float4(dout.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Displacement mapping
	dout.PosW.y = gHeightMap.SampleLevel(samHeightmap, dout.Tex, 0).r;

	// NOTE: We tried computing the normal in the shader using finite difference, 
	// but the vertices move continuously with fractional_even which creates
	// noticable light shimmering artifacts as the normal changes.  Therefore,
	// we moved the calculation to the pixel shader.  

	// Project to homogeneous clip space.
	dout.PosH = mul(float4(dout.PosW, 1.0f), gViewProj);

	return dout;
}

struct DomainOut
{
	float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
	float2 Tex      : TEXCOORD;
};

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("tri")]
DomainOut DS(PatchTess patchTess, 
             float3 bary : SV_DomainLocation, 
             const OutputPatch<HullOut,3> tri)
{
	DomainOut dout;
	
	// Interpolate patch attributes to generated vertices.
	dout.PosW     = bary.x*tri[0].PosW     + bary.y*tri[1].PosW     + bary.z*tri[2].PosW;
	dout.NormalW  = bary.x*tri[0].NormalW  + bary.y*tri[1].NormalW  + bary.z*tri[2].NormalW;
	dout.Tex      = bary.x*tri[0].Tex      + bary.y*tri[1].Tex      + bary.z*tri[2].Tex;
	
	// Interpolating normal can unnormalize it, so normalize it.
	dout.NormalW = normalize(dout.NormalW);
	
	//
	// Displacement mapping.
	//
	
	// Choose the mipmap level based on distance to the eye; specifically, choose
	// the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
	const float MipInterval = 20.0f;
	float mipLevel = clamp( (distance(dout.PosW, gEyePosW) - MipInterval) / MipInterval, 0.0f, 6.0f);
	
	// Sample height map (stored in alpha channel).
	float h = gNormalMap.SampleLevel(samLinear, dout.Tex, mipLevel).a;
	
	// Offset vertex along normal.
	dout.PosW += (gHeightScale*(h-1.0))*dout.NormalW;
	
	// Project to homogeneous clip space.
	dout.PosH = mul(float4(dout.PosW, 1.0f), gViewProj);
	
	return dout;
}

// This is only used for alpha cut out geometry, so that shadows 
// show up correctly.  Geometry that does not need to sample a
// texture can use a NULL pixel shader for depth pass.
void PS(VertexOut pin)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pin.Tex);

	// Don't write transparent pixels to the shadow map.
	clip(diffuse.a - 0.15f);
}

// This is only used for alpha cut out geometry, so that shadows 
// show up correctly.  Geometry that does not need to sample a
// texture can use a NULL pixel shader for depth pass.
void TessPS(DomainOut pin)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pin.Tex);

	// Don't write transparent pixels to the shadow map.
	clip(diffuse.a - 0.15f);
}

void TerrainPS(TerrainDomainOut pin)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pin.Tex);

	// Don't write transparent pixels to the shadow map.
	clip(diffuse.a - 0.15f);
}

RasterizerState Depth
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
	DepthBias = 100000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

technique11 BuildShadowMapTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( NULL );

		SetRasterizerState(Depth);
    }
}

technique11 BuildShadowMapSkinningTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinningVS()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);
		//SetPixelShader(CompileShader(ps_5_0, PS()));

		SetRasterizerState(Depth);
	}
}

technique11 BuildShadowMapAlphaClipTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}

technique11 BuildShadowMapSkinnedTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, SkinnedVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( NULL );

		SetRasterizerState(Depth);
    }
}

technique11 BuildShadowMapAlphaClipSkinnedTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, SkinnedVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}

technique11 TessBuildShadowMapTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, TessVS() ) );
		SetHullShader( CompileShader( hs_5_0, HS() ) );
        SetDomainShader( CompileShader( ds_5_0, DS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( NULL );

		SetRasterizerState(Depth);
    }
}

technique11 TessBuildShadowMapAlphaClipTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, TessVS() ) );
		SetHullShader( CompileShader( hs_5_0, HS() ) );
        SetDomainShader( CompileShader( ds_5_0, DS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, TessPS() ) );
    }
}

technique11 TerrainBuildShadowMapAlphaClipTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, TeerrainVS()));
		SetHullShader(CompileShader(hs_5_0, TerrainHS()));
		SetDomainShader(CompileShader(ds_5_0, TerrainDS()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(Depth);

	}
}