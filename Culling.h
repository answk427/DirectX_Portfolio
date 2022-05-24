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
	//������Ŀ��� �þ߰��� ����ü�� ����ϴ� �Լ�
	static void ComputeFrustumFromProjection(XNA::Frustum* pOut, XMMATRIX* pProjection);
	//������� ����ü�� ����ϴ� �Լ�
	static void ComputeFrustumInWorld(XNA::Frustum* pOut, XMMATRIX* pProjection, XMMATRIX* pWorld, XMMATRIX* pView);
	//�þ߰��� ����ü�� ���Ұ������� ��ȯ�ϴ� �Լ�
	static bool FrustumCulling::ConvertFrustumViewToLocal(XNA::Frustum * pOut, const XNA::Frustum * pViewFrustum, const XMMATRIX * pWorld, const XMMATRIX * pView);
	//�þ߰��� ����ü�� ����������� ��ȯ�ϴ� �Լ�
	static void ConvertFrustumViewToWorld(XNA::Frustum* pOut, XNA::Frustum* pViewFrustum, XMMATRIX* pView);

	//����ü ����(����ü ���� ��ü�� 0 ��ȯ)
	static INT ComputeFrustumCulling(XNA::AxisAlignedBox* aabb, Camera* camera, XMMATRIX* pWorld);
};


