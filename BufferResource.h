#pragma once
#include <d3dUtil.h>

namespace MyVertex
{
	//�������� ���Ǵ� ��������ü
	struct BasicVertex
	{
		XMFLOAT3 pos; //������ ��ġ
		XMFLOAT3 normal; //��������
		XMFLOAT2 tex; //�ؽ��� ��ǥ
		XMFLOAT3 tan; //ź��Ʈ(����) ����
		XMFLOAT3 biTan; //����ź��Ʈ(������) ����
		
		
		BasicVertex() :pos(0,0,0), normal(1,1,1), tex(0, 0), tan(0,0,0), biTan(0,0,0)  {}
		BasicVertex(float px, float py, float pz) : pos(px, py, pz), normal(1, 1, 1), tex(0, 0), tan(0, 0, 0), biTan(0, 0, 0) {}
		BasicVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: pos(px, py, pz), normal(nx, ny, nz),
			tan(tx, ty, tz), tex(u, v), biTan(0,0,0) {}
	};

	//���� �ִ� �𵨿� ���, �ٸ� ���۽��Կ� ������.
	struct SkinnedData
	{
		XMFLOAT3 weight; //�������� ����ġ, ������ ����ġ�� 1���� ���� ����
		BYTE boneIndices[4]; //�ִ� 4���� ���� ������ ����
	};
}

//�ٸ� ���۽��Կ� �� �ν��Ͻ��ڷ�
struct InstancingData
{
	XMFLOAT4X4 world; //������ǥ
	XMFLOAT4X4 worldInvTranspose; //����ġ���
	XMFLOAT4 color; //�⺻����
};

struct AABB_MaxMin
{
	XMFLOAT3 m_max;
	XMFLOAT3 m_min;

	AABB_MaxMin() : m_max(1, 1, 1), m_min(0, 0, 0) {}
	AABB_MaxMin(const XMFLOAT3& maxVector, const XMFLOAT3& minVector) :m_max(maxVector), m_min(minVector) {}
	
};