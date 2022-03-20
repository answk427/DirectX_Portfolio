#pragma once
#include <d3dUtil.h>

namespace MyVertex
{
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
		XMFLOAT3 weight; //뼈에대한 가중치, 마지막 가중치는 1에서 빼서 구함
		BYTE boneIndices[4]; //최대 4개의 뼈에 영향을 받음
	};
}

//다른 버퍼슬롯에 들어갈 인스턴싱자료
struct InstancingData
{
	XMFLOAT4X4 world; //월드좌표
	XMFLOAT4X4 worldInvTranspose; //역전치행렬
	XMFLOAT4 color; //기본색상
};

struct AABB_MaxMin
{
	XMFLOAT3 m_max;
	XMFLOAT3 m_min;

	AABB_MaxMin() : m_max(1, 1, 1), m_min(0, 0, 0) {}
	AABB_MaxMin(const XMFLOAT3& maxVector, const XMFLOAT3& minVector) :m_max(maxVector), m_min(minVector) {}
	
};