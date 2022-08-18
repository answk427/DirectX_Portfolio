#pragma once
#include <d3dUtil.h>


namespace MyVertex
{
	//정점버퍼, 인덱스버퍼를 1개씩 사용할 경우
	//(버퍼 하나에 여러 메쉬의 정점,인덱스를 몰아넣는 것)
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

	//공통으로 사용되는 정점구조체
	struct BasicVertex
	{
		XMFLOAT3 pos; //정점의 위치
		XMFLOAT3 normal; //법선벡터
		XMFLOAT2 tex; //텍스쳐 좌표
		XMFLOAT3 tan; //탄젠트(접선) 벡터
		XMFLOAT3 biTan; //바이탄젠트(종접선) 벡터
		
		
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

	//뼈가 있는 모델에 사용, 다른 버퍼슬롯에 넣을것.
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
		XMFLOAT3 weight; //뼈에대한 가중치, 마지막 가중치는 1에서 빼서 구함
		int boneIndices[4]; //최대 4개의 뼈에 영향을 받음
	};

	struct ComputedVertex
	{
		ComputedVertex() : pos(0.0f, 0.0f, 0.0f), posH(0.0f,0.0f,0.0f,1.0f), posW(0.0f,0.0f,0.0f),
			normalW(0.0f,0.0f,0.0f), tex(0.0f,0.0f), tanW(0.0f,0.0f,0.0f,0.0f), biTanW(0.0f,0.0f,0.0f, 0.0f)
		{} 
		XMFLOAT3 pos; //정점의 위치
		XMFLOAT4 posH;
		XMFLOAT3 posW;
		XMFLOAT3 normalW; //법선벡터
		XMFLOAT2 tex; //텍스쳐 좌표
		XMFLOAT4 tanW; //탄젠트(접선) 벡터
		XMFLOAT4 biTanW; //바이탄젠트(종접선) 벡터
	};
}

struct InstancingData
{
	InstancingData() : color{ 1.0f,1.0f,1.0f,1.0f } {}
	XMFLOAT4X4 world; //월드좌표
	XMFLOAT4X4 worldInvTranspose; //역전치행렬
	XMFLOAT4 color; //기본색상
	UINT RendererIdx; //어떤 렌더러에서 그렸는지 확인하기 위한 변수
};

//다른 버퍼슬롯에 들어갈 인스턴싱자료
struct InstancingMatrixData
{
	XMFLOAT4X4 world; //월드좌표
	XMFLOAT4X4 worldInvTranspose; //역전치행렬
};

struct InstancingBasicData
{
	InstancingBasicData() : color{ 1.0f,1.0f,1.0f,1.0f } {}
	XMFLOAT4 color; //기본색상
	UINT RendererIdx; //어떤 렌더러에서 그렸는지 확인하기 위한 변수
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