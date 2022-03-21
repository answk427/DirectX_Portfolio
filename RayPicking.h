#pragma once

#include <Camera.h>
#include <Renderer.h>
#include <D3D11.h>


class RayPicking
{
public:
	//��ũ����ǥ -> �þ߰��������� �������� ����ϴ� �Լ�
	static void ScreenToViewRay(XMVECTOR* rayOrigin, XMVECTOR* rayDir, float sx, float sy, D3D11_VIEWPORT * viewPort, XMMATRIX * projection);
	//�þ߰��� ������ -> ���Ұ��� ������ ��ȯ / ��ȯ�� origin,dir
	static std::pair<XMVECTOR, XMVECTOR> ViewToLocalRay(XMVECTOR* rayOrigin, XMVECTOR* rayDir, XMMATRIX* view, XMMATRIX* world);
	//���������� ���� ��, ���� ����� ��ü�� ��ȯ
	static Renderer* NearestOfIntersectRayAABB(D3D11_VIEWPORT* viewPort, 
		const std::vector<Renderer*>& objects,Camera* camera, float sx, float sy);
};
