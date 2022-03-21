#pragma once

#include <Camera.h>
#include <Renderer.h>
#include <D3D11.h>


class RayPicking
{
public:
	//스크린좌표 -> 시야공간에서의 반직선을 계산하는 함수
	static void ScreenToViewRay(XMVECTOR* rayOrigin, XMVECTOR* rayDir, float sx, float sy, D3D11_VIEWPORT * viewPort, XMMATRIX * projection);
	//시야공간 반직선 -> 국소공간 반직선 변환 / 반환값 origin,dir
	static std::pair<XMVECTOR, XMVECTOR> ViewToLocalRay(XMVECTOR* rayOrigin, XMVECTOR* rayDir, XMMATRIX* view, XMMATRIX* world);
	//반직선교차 판정 후, 가장 가까운 물체를 반환
	static Renderer* NearestOfIntersectRayAABB(D3D11_VIEWPORT* viewPort, 
		const std::vector<Renderer*>& objects,Camera* camera, float sx, float sy);
};
