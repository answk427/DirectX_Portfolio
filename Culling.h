#pragma once

#include "Camera.h"
#include <xnacollision.h>


class Frustum
{
public:
	Frustum(Camera* camera) : m_camera(camera) 
	{
		m_viewFrustum = new XNA::Frustum();
		m_worldFrustum = new XNA::Frustum();
	}
	~Frustum() 
	{ 
		delete m_viewFrustum; 
	delete m_worldFrustum;
	}

	Camera* m_camera;
	XNA::Frustum* m_viewFrustum;
	XNA::Frustum* m_worldFrustum;
public:
	void Update();
	INT LocalFrustumCulling(const XNA::AxisAlignedBox* localAABB, const XMMATRIX* pWorld);
	INT WorldFrustumCulling(const XNA::AxisAlignedBox* worldAABB);
};

class FrustumCulling
{
public:
	//투영행렬에서 시야공간 절두체를 계산하는 함수
	static void ComputeFrustumFromProjection(XNA::Frustum* pOut, XMMATRIX* pProjection);
	//세계공간 절두체를 계산하는 함수
	static void ComputeFrustumInWorld(XNA::Frustum* pOut, XMMATRIX* pProjection, XMMATRIX* pWorld, XMMATRIX* pView);
	//시야공간 절두체를 국소공간으로 변환하는 함수
	static bool FrustumCulling::ConvertFrustumViewToLocal(XNA::Frustum * pOut, const XNA::Frustum * pViewFrustum, const XMMATRIX * pWorld, const XMMATRIX * pView);
	//시야공간 절두체를 세계공간으로 변환하는 함수
	static void ConvertFrustumViewToWorld(XNA::Frustum* pOut, XNA::Frustum* pViewFrustum, XMMATRIX* pView);

	//절두체 선별(절두체 밖의 물체는 0 반환)
	static INT ComputeFrustumCulling(XNA::AxisAlignedBox* aabb, Camera* camera, XMMATRIX* pWorld);
};


