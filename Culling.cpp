#include "Culling.h"

//�ռ���� �������� ������ĸ� �־�� ��.
void FrustumCulling::ComputeFrustumFromProjection(XNA::Frustum * pOut, XMMATRIX * pProjection)
{
	XMASSERT(pOut);
	XMASSERT(pProjection);

	//�������ܰ��������� ���� ����ü ��������
	static XMVECTOR HomogeneousPoints[6] =
	{
		{1.0f, 0.0f, 1.0f, 1.0f}, //������ (�����)
		{-1.0f, 0.0f, 1.0f, 1.0f}, //����
		{0.0f, 1.0f, 1.0f, 1.0f}, //��
		{0.0f, -1.0f, 1.0f, 1.0f},//�Ʒ�
		
		{0.0f, 0.0f, 0.0f, 1.0f}, //�����
		{0.0f, 0.0f, 1.0f, 1.0f} //��
	};

	XMVECTOR Determinant;
	//���������
	XMMATRIX matInverse = XMMatrixInverse(&Determinant, *pProjection);

	//�þ� ���������� ����ü ��������
	XMVECTOR Points[6];

	for (int i = 0; i < 6; i++)
	{
		Points[i] = XMVector4Transform(HomogeneousPoints[i], matInverse);
	}

	pOut->Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pOut->Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //ȸ�� �����

	//������� ���(�¿���� ���), XMVectorReciprocal = ����
	Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0]));
	Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
	Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
	Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));
	
	pOut->RightSlope = XMVectorGetX(Points[0]);
	pOut->LeftSlope = XMVectorGetX(Points[1]);
	pOut->TopSlope = XMVectorGetY(Points[2]);
	pOut->BottomSlope = XMVectorGetY(Points[3]);

	//����� ���Ÿ�, �� ��� �Ÿ� ���
	Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
	Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

	pOut->Near = XMVectorGetZ(Points[4]);
	pOut->Far = XMVectorGetZ(Points[5]);

	return;
}

void FrustumCulling::ConvertFrustumViewToLocal(XNA::Frustum * pOut, XNA::Frustum * pViewFrustum, XMMATRIX * pWorld, XMMATRIX * pView)
{
	//�þ� ����� ����� ���
	XMVECTOR detView = XMMatrixDeterminant(*pView);
	XMMATRIX invView = XMMatrixInverse(&detView, *pView);

	//��������� ����� ���
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(*pWorld), *pWorld);

	//�þ߰���->���Ұ��� ��ȯ��� ���
	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	//����� ���� ��ȯ��� �����Ѵ�.
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

	//����ü�� �þ߰��� -> ���Ұ������� ��ȯ
	XNA::TransformFrustum(pOut, pViewFrustum, XMVectorGetX(scale), rotQuat, translation);
	
}

INT FrustumCulling::ComputeFrustumCulling(XNA::AxisAlignedBox* aabb, Camera * camera, XMMATRIX * pWorld)
{
	//�þ߰��� ����ü ���
	XNA::Frustum viewFrustum;
	ComputeFrustumFromProjection(&viewFrustum, &camera->Proj());
	
	//���Ұ��� ����ü ���
	XNA::Frustum localFrustum;
	ConvertFrustumViewToLocal(&localFrustum, &viewFrustum, pWorld, &camera->View());
		
	return XNA::IntersectAxisAlignedBoxFrustum(aabb, &localFrustum);
}
