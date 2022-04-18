//***************************************************************************************
// ShadowMap.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper class for implementing shadows via shadow mapping algorithm.
//***************************************************************************************

#ifndef SHADOW_MAPPER_H
#define SHADOW_MAPPER_H

#include "d3dUtil.h"
#include "Camera.h"
#include "EffectMgr.h"
#include "Renderer.h"

struct BoundingSphere
{
	BoundingSphere() : center(0.0f, 0.0f, 0.0f), radius(0.0f) {}
	XMFLOAT3 center;
	float radius;
};

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, UINT width, UINT height, const std::wstring& shaderName);
	~ShadowMap();
	UINT mWidth;
	ID3D11ShaderResourceView* DepthMapSRV();
	std::wstring m_shaderName;
	BoundingSphere m_boundingSphere;
	
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	std::unique_ptr<Camera> m_shadowMapCamera;
public:
	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	//세계공간에서 모든 정점을 포함하는 구를 생성하는 함수
	void ComputeBoundingSphere(std::vector<Renderer*> renderers);
	bool BuildShadowTransform(const DirectionalLight& dirLight, XMVECTOR& worldUp);
	
	

private:
	ShadowMap(const ShadowMap& rhs);
	ShadowMap& operator=(const ShadowMap& rhs);

private:
	UINT mHeight;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;
};

#endif // SHADOW_MAPPER_H