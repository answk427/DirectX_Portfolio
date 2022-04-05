#include "Culling.h"

//합성행렬 넣지말고 투영행렬만 넣어야 함.
void FrustumCulling::ComputeFrustumFromProjection(XNA::Frustum * pOut, XMMATRIX * pProjection)
{
	XMASSERT(pOut);
	XMASSERT(pProjection);

	//동차절단공간에서의 투영 절두체 꼭지점들
	static XMVECTOR HomogeneousPoints[6] =
	{
		{1.0f, 0.0f, 1.0f, 1.0f}, //오른쪽 (먼평면)
		{-1.0f, 0.0f, 1.0f, 1.0f}, //왼쪽
		{0.0f, 1.0f, 1.0f, 1.0f}, //위
		{0.0f, -1.0f, 1.0f, 1.0f},//아래
		
		{0.0f, 0.0f, 0.0f, 1.0f}, //가까운
		{0.0f, 0.0f, 1.0f, 1.0f} //먼
	};

	XMVECTOR Determinant;
	//역투영행렬
	XMMATRIX matInverse = XMMatrixInverse(&Determinant, *pProjection);

	//시야 공간에서의 절두체 꼭짓점들
	XMVECTOR Points[6];

	for (int i = 0; i < 6; i++)
	{
		Points[i] = XMVector4Transform(HomogeneousPoints[i], matInverse);
	}

	pOut->Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pOut->Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //회전 사원수

	//기울기들을 계산(좌우상하 평면), XMVectorReciprocal = 역수
	Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0]));
	Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
	Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
	Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));
	
	pOut->RightSlope = XMVectorGetX(Points[0]);
	pOut->LeftSlope = XMVectorGetX(Points[1]);
	pOut->TopSlope = XMVectorGetY(Points[2]);
	pOut->BottomSlope = XMVectorGetY(Points[3]);

	//가까운 평면거리, 먼 평먼 거리 계산
	Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
	Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

	pOut->Near = XMVectorGetZ(Points[4]);
	pOut->Far = XMVectorGetZ(Points[5]);

	return;
}

void FrustumCulling::ComputeFrustumInWorld(XNA::Frustum * pOut, XMMATRIX * pProjection, XMMATRIX * pWorld, XMMATRIX * pView)
{
	//시야공간에서의 절두체를 구한다.
	XNA::Frustum viewFrustum;
	ComputeFrustumFromProjection(&viewFrustum, pProjection);
		
	
	//세계행렬의 역행렬
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(*pWorld), *pWorld);

	//행렬을 개별 변환들로 분해한다.
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scale, &rotQuat, &translation, invWorld);
	
	//시야공간->세계공간으로 변환한다.
	XNA::TransformFrustum(pOut, &viewFrustum, XMVectorGetX(scale), rotQuat, translation);
}

void FrustumCulling::ConvertFrustumViewToLocal(XNA::Frustum * pOut, const XNA::Frustum * pViewFrustum, const XMMATRIX * pWorld, const XMMATRIX * pView)
{
	//시야 행렬의 역행렬 계산
	XMVECTOR detView = XMMatrixDeterminant(*pView);
	XMMATRIX invView = XMMatrixInverse(&detView, *pView);

	//세계행렬의 역행렬 계산
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(*pWorld), *pWorld);

	//시야공간->국소공간 변환행렬 계산
	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	//행렬을 개별 변환들로 분해한다.
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

	//절두체를 시야공간 -> 국소공간으로 변환
	XNA::TransformFrustum(pOut, pViewFrustum, XMVectorGetX(scale), rotQuat, translation);
	
}

void FrustumCulling::ConvertFrustumViewToWorld(XNA::Frustum * pOut, XNA::Frustum * pViewFrustum, XMMATRIX * pView)
{
	//시야 행렬의 역행렬 계산
	XMVECTOR detView = XMMatrixDeterminant(*pView);
	XMMATRIX invView = XMMatrixInverse(&detView, *pView);

	//행렬을 개별 변환들로 분해한다.
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scale, &rotQuat, &translation, invView);

	//절두체를 시야공간 -> 국소공간으로 변환
	XNA::TransformFrustum(pOut, pViewFrustum, XMVectorGetX(scale), rotQuat, translation);
}

INT FrustumCulling::ComputeFrustumCulling(XNA::AxisAlignedBox* aabb, Camera * camera, XMMATRIX * pWorld)
{
	//시야공간 절두체 계산
	XNA::Frustum viewFrustum;
	ComputeFrustumFromProjection(&viewFrustum, &camera->Proj());
	
	//국소공간 절두체 계산
	XNA::Frustum localFrustum;
	ConvertFrustumViewToLocal(&localFrustum, &viewFrustum, pWorld, &camera->View());
		
	return XNA::IntersectAxisAlignedBoxFrustum(aabb, &localFrustum);
}

void Frustum::Update()
{
	//시야공간 절두체 갱신
	FrustumCulling::ComputeFrustumFromProjection(m_viewFrustum, &m_camera->Proj());
	//세계공간 절두체 갱신
	FrustumCulling::ConvertFrustumViewToWorld(m_worldFrustum, m_viewFrustum, &m_camera->View());
}

INT Frustum::LocalFrustumCulling(const XNA::AxisAlignedBox * localAABB, const XMMATRIX * pWorld)
{
	XNA::Frustum localFrustum;
	FrustumCulling::ConvertFrustumViewToLocal(&localFrustum, m_viewFrustum, pWorld, &m_camera->View());
		
	return XNA::IntersectAxisAlignedBoxFrustum(localAABB, &localFrustum);
}

INT Frustum::WorldFrustumCulling(const XNA::AxisAlignedBox * worldAABB)
{
	return XNA::IntersectAxisAlignedBoxFrustum(worldAABB, m_worldFrustum);
}
