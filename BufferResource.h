#pragma once
#include <d3dUtil.h>

namespace MyVertex
{
	//�������� ���Ǵ� ��������ü
	struct BasicVertex
	{
		XMFLOAT3 pos; //������ ��ġ
		XMFLOAT3 normal; //��������
		XMFLOAT3 tan; //ź��Ʈ ����
		XMFLOAT2 tex; //�ؽ��� ��ǥ
		
		BasicVertex() :pos(0), normal(0), tan(0), tex(0) {}
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
	XMCOLOR color; //�⺻����
	XMMATRIX world; //������ǥ
};