#include "RayPicking.h"

void RayPicking::ScreenToViewRay(XMVECTOR* rayOrigin, XMVECTOR* rayDir, float sx, float sy, D3D11_VIEWPORT * viewPort, XMMATRIX * projection)
{
	//�þ� �������� ���� �������� ���
	float vx = (+2.0f*(sx - viewPort->TopLeftX)/viewPort->Width - 1.0f) / (*projection)(0, 0);
	float vy = (+2.0f*(viewPort->TopLeftY - sy) / viewPort->Height + 1.0f) / (*projection)(1, 1);

	//�þ� ���� ������ ����
	*rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	*rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);
}

std::pair<XMVECTOR,XMVECTOR> RayPicking::ViewToLocalRay(XMVECTOR * rayOrigin, XMVECTOR * rayDir, XMMATRIX * view, XMMATRIX * world)
{
	//�þ������ �����
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(*view), *view);
	//��������� �����
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(*world), *world);

	//�þ߰���->���ð������� ��ȯ�ϴ� ���
	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	std::pair<XMVECTOR, XMVECTOR> localRay;
	localRay.first = XMVector3TransformCoord(*rayOrigin, toLocal); //����������
	localRay.second = XMVector3TransformNormal(*rayDir, toLocal); //������ ����

	//���� ������ ���� ������ ���⺤�͸� �������̷� �����
	localRay.second = XMVector3Normalize(localRay.second);

	return localRay;
}

Renderer * RayPicking::NearestOfIntersectRayAABB(D3D11_VIEWPORT* viewPort ,
	const std::vector<Renderer*>& objects, Camera * camera, float sx, float sy)
{
	Renderer* result = nullptr;

	XMVECTOR rayOrigin;
	XMVECTOR rayDir;

	//��ũ����ǥ -> �þ߰��� ������ ���
	ScreenToViewRay(&rayOrigin, &rayDir, sx, sy, viewPort, &camera->Proj());
	
	float tMin = INT_MAX;
	for (auto elem : objects)
	{
		std::pair<XMVECTOR, XMVECTOR> localRay;
		//�þ߰��� ������ -> ���Ұ��� ������ ��ȯ
		localRay = ViewToLocalRay(&rayOrigin, &rayDir, &camera->View(), 
			&XMLoadFloat4x4(elem->GetTransform()->GetWorld()));
		
		//�������� AABB ��������
		float t = 0.0f;
		if (XNA::IntersectRayAxisAlignedBox(localRay.first, localRay.second, &elem->GetMesh()->GetAABB(), &t))
		{
			//�������� �� ���� ����� ������Ʈ�� ã�´�.
			if (t < tMin)
			{
				tMin = t;
				result = elem;
			}	
		}
	}
	//�����ϴ� ���� ���ٸ� nullptr ��ȯ
	return result;
}
