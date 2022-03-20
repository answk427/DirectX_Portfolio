#pragma once

#include "Camera.h"
#include <xnacollision.h>

class FrustumCulling
{
public:
	//투영행렬에서 시야공간 절두체를 계산하는 함수
	static void ComputeFrustumFromProjection(XNA::Frustum* pOut, XMMATRIX* pProjection);
	//시야공간 절두체를 국소공간으로 변환하는 함수
	static void ConvertFrustumViewToLocal(XNA::Frustum* pOut, XNA::Frustum* pViewFrustum, XMMATRIX* pWorld, XMMATRIX* pView);
	//절두체 선별(절두체 밖의 물체는 0 반환)
	static INT ComputeFrustumCulling(XNA::AxisAlignedBox* aabb, Camera* camera, XMMATRIX* pWorld);
};


