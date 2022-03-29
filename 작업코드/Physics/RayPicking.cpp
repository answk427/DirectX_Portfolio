#include "RayPicking.h"

void RayPicking::ScreenToViewRay(XMVECTOR* rayOrigin, XMVECTOR* rayDir, float sx, float sy, D3D11_VIEWPORT * viewPort, XMMATRIX * projection)
{
	//시야 공간에서 선택 반직선을 계산
	float vx = (+2.0f*(sx - viewPort->TopLeftX)/viewPort->Width - 1.0f) / (*projection)(0, 0);
	float vy = (+2.0f*(viewPort->TopLeftY - sy) / viewPort->Height + 1.0f) / (*projection)(1, 1);

	//시야 공간 반직선 정의
	*rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	*rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);
}

std::pair<XMVECTOR,XMVECTOR> RayPicking::ViewToLocalRay(XMVECTOR * rayOrigin, XMVECTOR * rayDir, XMMATRIX * view, XMMATRIX * world)
{
	//시야행렬의 역행렬
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(*view), *view);
	//세계행렬의 역행렬
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(*world), *world);

	//시야공간->로컬공간으로 변환하는 행렬
	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	std::pair<XMVECTOR, XMVECTOR> localRay;
	localRay.first = XMVector3TransformCoord(*rayOrigin, toLocal); //반직선원점
	localRay.second = XMVector3TransformNormal(*rayDir, toLocal); //반직선 방향

	//교차 판정을 위해 반직선 방향벡터를 단위길이로 만든다
	localRay.second = XMVector3Normalize(localRay.second);

	return localRay;
}

Renderer * RayPicking::NearestOfIntersectRayAABB(D3D11_VIEWPORT* viewPort ,
	const std::vector<Renderer*>& objects, Camera * camera, float sx, float sy)
{
	Renderer* result = nullptr;

	XMVECTOR rayOrigin;
	XMVECTOR rayDir;

	//스크린좌표 -> 시야공간 반직선 계산
	ScreenToViewRay(&rayOrigin, &rayDir, sx, sy, viewPort, &camera->Proj());
	
	float tMin = INT_MAX;
	for (auto elem : objects)
	{
		std::pair<XMVECTOR, XMVECTOR> localRay;
		//시야공간 반직선 -> 국소공간 반직선 변환
		localRay = ViewToLocalRay(&rayOrigin, &rayDir, &camera->View(), 
			&XMLoadFloat4x4(elem->GetTransform()->GetWorld()));
		
		//반직선과 AABB 교차판정
		float t = 0.0f;
		if (XNA::IntersectRayAxisAlignedBox(localRay.first, localRay.second, &elem->GetMesh()->GetAABB(), &t))
		{
			//교차했을 때 가장 가까운 오브젝트를 찾는다.
			if (t < tMin)
			{
				tMin = t;
				result = elem;
			}	
		}
	}
	//교차하는 것이 없다면 nullptr 반환
	return result;
}
