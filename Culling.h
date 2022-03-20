#pragma once

#include "Camera.h"
#include <xnacollision.h>

class FrustumCulling
{
public:
	//������Ŀ��� �þ߰��� ����ü�� ����ϴ� �Լ�
	static void ComputeFrustumFromProjection(XNA::Frustum* pOut, XMMATRIX* pProjection);
	//�þ߰��� ����ü�� ���Ұ������� ��ȯ�ϴ� �Լ�
	static void ConvertFrustumViewToLocal(XNA::Frustum* pOut, XNA::Frustum* pViewFrustum, XMMATRIX* pWorld, XMMATRIX* pView);
	//����ü ����(����ü ���� ��ü�� 0 ��ȯ)
	static INT ComputeFrustumCulling(XNA::AxisAlignedBox* aabb, Camera* camera, XMMATRIX* pWorld);
};


