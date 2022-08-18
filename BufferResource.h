#pragma once
#include <d3dUtil.h>


namespace MyVertex
{
	//��������, �ε������۸� 1���� ����� ���
	//(���� �ϳ��� ���� �޽��� ����,�ε����� ���Ƴִ� ��)
	class Subset
	{
	public:
		UINT VertexCount;
		UINT VertexStart;
		UINT IndexCount;
		UINT IndexStart;
		UINT materialNum;

		Subset() : VertexCount(0), VertexStart(0),
			IndexCount(0), IndexStart(0), materialNum(0) {}
	};

	struct PosColorVertex
	{
		PosColorVertex(float x, float y, float z) : pos(x, y, z), color(0.0f, 1.0f, 0.0f, 1.0f) {}
		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	struct Terrain
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
		XMFLOAT2 BoundsY;
	};

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
		SkinnedData() :weight(0.0f, 0.0f, 0.0f), boneIndices{ -1, -1, -1,-1 } {}
		SkinnedData(const std::vector<float>& weights, const std::vector<int>& indices) : SkinnedData()
		{
			for (int i = 0; i < weights.size(); ++i)
			{
				if (i == 0)
					weight.x = weights[i];
				else if (i == 1)
					weight.y = weights[i];
				else if (i == 2)
					weight.z = weights[i];
				
				boneIndices[i] = indices[i];
			}
		}
		XMFLOAT3 weight; //�������� ����ġ, ������ ����ġ�� 1���� ���� ����
		int boneIndices[4]; //�ִ� 4���� ���� ������ ����
	};

	struct ComputedVertex
	{
		ComputedVertex() : pos(0.0f, 0.0f, 0.0f), posH(0.0f,0.0f,0.0f,1.0f), posW(0.0f,0.0f,0.0f),
			normalW(0.0f,0.0f,0.0f), tex(0.0f,0.0f), tanW(0.0f,0.0f,0.0f,0.0f), biTanW(0.0f,0.0f,0.0f, 0.0f)
		{} 
		XMFLOAT3 pos; //������ ��ġ
		XMFLOAT4 posH;
		XMFLOAT3 posW;
		XMFLOAT3 normalW; //��������
		XMFLOAT2 tex; //�ؽ��� ��ǥ
		XMFLOAT4 tanW; //ź��Ʈ(����) ����
		XMFLOAT4 biTanW; //����ź��Ʈ(������) ����
	};
}

struct InstancingData
{
	InstancingData() : color{ 1.0f,1.0f,1.0f,1.0f } {}
	XMFLOAT4X4 world; //������ǥ
	XMFLOAT4X4 worldInvTranspose; //����ġ���
	XMFLOAT4 color; //�⺻����
	UINT RendererIdx; //� ���������� �׷ȴ��� Ȯ���ϱ� ���� ����
};

//�ٸ� ���۽��Կ� �� �ν��Ͻ��ڷ�
struct InstancingMatrixData
{
	XMFLOAT4X4 world; //������ǥ
	XMFLOAT4X4 worldInvTranspose; //����ġ���
};

struct InstancingBasicData
{
	InstancingBasicData() : color{ 1.0f,1.0f,1.0f,1.0f } {}
	XMFLOAT4 color; //�⺻����
	UINT RendererIdx; //� ���������� �׷ȴ��� Ȯ���ϱ� ���� ����
};

struct AABB_MaxMin
{
	XMFLOAT3 m_max;
	XMFLOAT3 m_min;

	AABB_MaxMin() : m_max(1, 1, 1), m_min(0, 0, 0) {}
	AABB_MaxMin(const XMFLOAT3& maxVector, const XMFLOAT3& minVector) :m_max(maxVector), m_min(minVector) {}
	
};

enum BrushShape
{
	NOBRUSH,
	SQUARE,
	CIRCLE
};

struct BrushDesc
{
	BrushDesc() :color(0.0f, 1.0f, 0.0f, 1.0f), shape(BrushShape::NOBRUSH), centerW(0.0f,0.0f,0.0f), radius(10.0f)
	{}
	XMFLOAT4 color;
	XMFLOAT3 centerW;
	FLOAT radius;
	UINT shape;
	FLOAT padding[3];
};