//***************************************************************************************
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtil.h"
#include "Material.h"
#include <Camera.h>
#include "BufferResource.h"
#include "Light.h"
#include "Vertex.h"


enum TechniqueType
{
	Light = 1,
	DiffuseMap = 2,
	AlphaClip = 4,
	Fog = 8,
	Reflect = 16,
	Skinned = 32,
	Instancing = 64
};

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();
public:
	//렌더링에 필요한 
	virtual void Init(ID3D11Device* device);
	//device로부터 BlendState를 생성하는 함수
	virtual void InitBlendState(ID3D11Device* device);
	//device로부터 inputlayout을 생성하는 함수
	virtual void InitInputLayout(ID3D11Device* device) = 0;

	virtual void InitInstancingInputLayout(ID3D11Device* device) = 0;
	
	//Frame별로 필요한 세팅을 수행하는 함수
	virtual void PerFrameSet(DirectionalLight* directL,
		PointLight* pointL,
		SpotLight* spotL,
		const XMFLOAT3& eyePosW) = 0;
	
	//Object별로 필요한 세팅을 수행하는 함수
	virtual void PerObjectSet(GeneralMaterial* material,
		Camera* camera,
		CXMMATRIX& world) = 0;

	//Technique을 얻는 함수
	virtual ID3DX11EffectTechnique* GetTechnique(UINT techType) = 0;

	//렌더링파이프라인 세팅함수
	virtual bool IASetting(ID3D11DeviceContext* context, UINT techType);
	virtual bool OMSetting(ID3D11DeviceContext* context, bool blending);
	
	
	//텍스쳐 맵 세팅
	virtual void SetMaps(ID3D11ShaderResourceView* diffuseMap,
		ID3D11ShaderResourceView* normalMap,
		ID3D11ShaderResourceView* specularMap) {}

	
private:
	//복사를 못하게 함
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
	ID3D11InputLayout* m_inputLayout;
	ID3D11InputLayout* m_instancing_inputLayout;
	ID3D11BlendState* m_blendState;

public:
	void SetBlendState(ID3D11BlendState* blendState) { m_blendState = blendState; }

};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetViewProj(CXMMATRIX M)						{ ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	
	void SetDirLights(const DirectionalLight* lights)   
	{ 
		if (lights == nullptr)
		{
			dirLightSize->SetInt(0);
			return;
		}
		DirLights->SetRawValue(lights, 
			0,
			Lighting::lightCount[LightType::DIRECTIONAL] *sizeof(DirectionalLight)); 
		dirLightSize->SetInt(Lighting::lightCount[LightType::DIRECTIONAL]);
	}
	void SetPointLights(const PointLight* lights)
	{
		if (lights == nullptr)
		{
			pointLightSize->SetInt(0);
			return;
		}
		pointLights->SetRawValue(lights,
			0,
			Lighting::lightCount[LightType::POINTLIGHT] * sizeof(PointLight));
		pointLightSize->SetInt(Lighting::lightCount[LightType::POINTLIGHT]);

	}
	void SetSpotLights(const SpotLight* lights)
	{
		if (lights == nullptr)
		{
			spotLightSize->SetInt(0);
			return;
		}
		spotLights->SetRawValue(lights,
			0,
			Lighting::lightCount[LightType::SPOTLIGHT] * sizeof(SpotLight));
		spotLightSize->SetInt(Lighting::lightCount[LightType::SPOTLIGHT]);

	}
	void SetMaterial(const BasicMaterial& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }
	void SetSsaoMap(ID3D11ShaderResourceView* tex)      { SsaoMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;
	
	//인스턴싱 technique
	ID3DX11EffectTechnique* Light3TexInstancingTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	
	ID3DX11EffectScalarVariable* dirLightSize;

	ID3DX11EffectVariable* pointLights;
	ID3DX11EffectScalarVariable* pointLightSize;

	ID3DX11EffectVariable* spotLights;
	ID3DX11EffectScalarVariable* spotLightSize;



	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;

	// Effect을(를) 통해 상속됨
	virtual void PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const XMFLOAT3& eyePosW) override;
	virtual void PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX& world) override;
	virtual void SetMaps(ID3D11ShaderResourceView* diffuseMap,
		ID3D11ShaderResourceView* normalMap,
		ID3D11ShaderResourceView* specularMap);

	// Effect을(를) 통해 상속됨
	virtual ID3DX11EffectTechnique * GetTechnique(UINT techType) override;
		
	// Effect을(를) 통해 상속됨
	virtual void InitInputLayout(ID3D11Device * device) override;

	// Effect을(를) 통해 상속됨
	virtual void InitInstancingInputLayout(ID3D11Device * device) override;
};
#pragma endregion

#pragma region NormalMapEffect
class NormalMapEffect : public Effect
{
public:
	NormalMapEffect(ID3D11Device* device, const std::wstring& filename);
	~NormalMapEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }
	void SetSsaoMap(ID3D11ShaderResourceView* tex)      { SsaoMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectTechnique* Light1SkinnedTech;
	ID3DX11EffectTechnique* Light2SkinnedTech;
	ID3DX11EffectTechnique* Light3SkinnedTech;

	ID3DX11EffectTechnique* Light0TexSkinnedTech;
	ID3DX11EffectTechnique* Light1TexSkinnedTech;
	ID3DX11EffectTechnique* Light2TexSkinnedTech;
	ID3DX11EffectTechnique* Light3TexSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipSkinnedTech;

	ID3DX11EffectTechnique* Light1FogSkinnedTech;
	ID3DX11EffectTechnique* Light2FogSkinnedTech;
	ID3DX11EffectTechnique* Light3FogSkinnedTech;

	ID3DX11EffectTechnique* Light0TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light1TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light2TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light3TexFogSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogSkinnedTech;

	ID3DX11EffectTechnique* Light1ReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2ReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3ReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectSkinnedTech;

	ID3DX11EffectTechnique* Light1FogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2FogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3FogReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexFogReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectSkinnedTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* BoneTransforms;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;


	// Effect을(를) 통해 상속됨
	virtual void PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const XMFLOAT3 & eyePosW) override;

	virtual void PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world) override;


	// Effect을(를) 통해 상속됨
	virtual ID3DX11EffectTechnique * GetTechnique(UINT techType) override;

	


	// Effect을(를) 통해 상속됨
	virtual void InitInputLayout(ID3D11Device * device) override;

};
#pragma endregion

#pragma region BuildShadowMapEffect
class BuildShadowMapEffect : public Effect
{
public:
	BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename);
	~BuildShadowMapEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	
	void SetHeightScale(float f)                        { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	ID3DX11EffectTechnique* BuildShadowMapTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique* BuildShadowMapSkinnedTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* TessBuildShadowMapTech;
	ID3DX11EffectTechnique* TessBuildShadowMapAlphaClipTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* BoneTransforms;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;
 
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};
#pragma endregion

#pragma region SsaoNormalDepthEffect
class SsaoNormalDepthEffect : public Effect
{
public:
	SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoNormalDepthEffect();

	void SetWorldView(CXMMATRIX M)                      { WorldView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTransposeView(CXMMATRIX M)          { WorldInvTransposeView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* NormalDepthTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipTech;

	ID3DX11EffectTechnique* NormalDepthSkinnedTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipSkinnedTech;

	ID3DX11EffectMatrixVariable* WorldView;
	ID3DX11EffectMatrixVariable* WorldInvTransposeView;
	ID3DX11EffectMatrixVariable* BoneTransforms;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};
#pragma endregion

#pragma region SsaoEffect
class SsaoEffect : public Effect
{
public:
	SsaoEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoEffect();

	void SetViewToTexSpace(CXMMATRIX M)                    { ViewToTexSpace->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetOffsetVectors(const XMFLOAT4 v[14])            { OffsetVectors->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 14); }
	void SetFrustumCorners(const XMFLOAT4 v[4])            { FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4); }
	void SetOcclusionRadius(float f)                       { OcclusionRadius->SetFloat(f); }
	void SetOcclusionFadeStart(float f)                    { OcclusionFadeStart->SetFloat(f); }
	void SetOcclusionFadeEnd(float f)                      { OcclusionFadeEnd->SetFloat(f); }
	void SetSurfaceEpsilon(float f)                        { SurfaceEpsilon->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetRandomVecMap(ID3D11ShaderResourceView* srv)    { RandomVecMap->SetResource(srv); }

	ID3DX11EffectTechnique* SsaoTech;

	ID3DX11EffectMatrixVariable* ViewToTexSpace;
	ID3DX11EffectVectorVariable* OffsetVectors;
	ID3DX11EffectVectorVariable* FrustumCorners;
	ID3DX11EffectScalarVariable* OcclusionRadius;
	ID3DX11EffectScalarVariable* OcclusionFadeStart;
	ID3DX11EffectScalarVariable* OcclusionFadeEnd;
	ID3DX11EffectScalarVariable* SurfaceEpsilon;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* RandomVecMap;
};
#pragma endregion

#pragma region SsaoBlurEffect
class SsaoBlurEffect : public Effect
{
public:
	SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoBlurEffect();

	void SetTexelWidth(float f)                            { TexelWidth->SetFloat(f); }
	void SetTexelHeight(float f)                           { TexelHeight->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetInputImage(ID3D11ShaderResourceView* srv)      { InputImage->SetResource(srv); }

	ID3DX11EffectTechnique* HorzBlurTech;
	ID3DX11EffectTechnique* VertBlurTech;

	ID3DX11EffectScalarVariable* TexelWidth;
	ID3DX11EffectScalarVariable* TexelHeight;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* InputImage;
};
#pragma endregion

#pragma region SkyEffect
class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetCubeMap(ID3D11ShaderResourceView* cubemap)  { CubeMap->SetResource(cubemap); }

	ID3DX11EffectTechnique* SkyTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;

	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion

#pragma region DebugTexEffect
class DebugTexEffect : public Effect
{
public:
	DebugTexEffect(ID3D11Device* device, const std::wstring& filename);
	~DebugTexEffect();

	void SetWorldViewProj(CXMMATRIX M)              { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex)  { Texture->SetResource(tex); }

	ID3DX11EffectTechnique* ViewArgbTech;
	ID3DX11EffectTechnique* ViewRedTech;
	ID3DX11EffectTechnique* ViewGreenTech;
	ID3DX11EffectTechnique* ViewBlueTech;
	ID3DX11EffectTechnique* ViewAlphaTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectShaderResourceVariable* Texture;
};
#pragma endregion



#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;
	static NormalMapEffect* NormalMapFX;
	static BuildShadowMapEffect* BuildShadowMapFX;
	static SsaoNormalDepthEffect* SsaoNormalDepthFX;
	static SsaoEffect* SsaoFX;
	static SsaoBlurEffect* SsaoBlurFX;
	static SkyEffect* SkyFX;
	static DebugTexEffect* DebugTexFX;
};
#pragma endregion

class TreebilboardEffect : public Effect
{
public:
	TreebilboardEffect(ID3D11Device* device, const std::wstring& filename);
protected:
	ID3DX11EffectTechnique* Light3Tech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;
	

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectScalarVariable* dirLightSize;

	ID3DX11EffectVariable* pointLights;
	ID3DX11EffectScalarVariable* pointLightSize;

	ID3DX11EffectVariable* spotLights;
	ID3DX11EffectScalarVariable* spotLightSize;

	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* TreeTextureMapArray;

public:
	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v) { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f) { FogStart->SetFloat(f); }
	void SetFogRange(float f) { FogRange->SetFloat(f); }

	void SetMaterial(const BasicMaterial& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetTreeTextureMapArray(ID3D11ShaderResourceView* tex) { TreeTextureMapArray->SetResource(tex); }

	void SetDirLights(const DirectionalLight* lights)
	{
		if (lights == nullptr)
		{
			dirLightSize->SetInt(0);
			return;
		}
		DirLights->SetRawValue(lights,
			0,
			Lighting::lightCount[LightType::DIRECTIONAL] * sizeof(DirectionalLight));
		dirLightSize->SetInt(Lighting::lightCount[LightType::DIRECTIONAL]);
	}
	void SetPointLights(const PointLight* lights)
	{
		if (lights == nullptr)
		{
			pointLightSize->SetInt(0);
			return;
		}
		pointLights->SetRawValue(lights,
			0,
			Lighting::lightCount[LightType::POINTLIGHT] * sizeof(PointLight));
		pointLightSize->SetInt(Lighting::lightCount[LightType::POINTLIGHT]);

	}
	void SetSpotLights(const SpotLight* lights)
	{
		if (lights == nullptr)
		{
			spotLightSize->SetInt(0);
			return;
		}
		spotLights->SetRawValue(lights,
			0,
			Lighting::lightCount[LightType::SPOTLIGHT] * sizeof(SpotLight));
		spotLightSize->SetInt(Lighting::lightCount[LightType::SPOTLIGHT]);

	}

	


public:
	// Effect을(를) 통해 상속됨
	virtual void InitInputLayout(ID3D11Device * device) override;
	virtual void InitInstancingInputLayout(ID3D11Device * device) override;
	virtual void PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const XMFLOAT3 & eyePosW) override;
	virtual void PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world) override;
	virtual ID3DX11EffectTechnique * GetTechnique(UINT techType) override;
};

#endif // EFFECTS_H