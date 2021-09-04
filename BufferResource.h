#pragma once
#include <d3dUtil.h>

namespace MyVertex
{
	//공통으로 사용되는 정점구조체
	struct BasicVertex
	{
		XMFLOAT3 pos; //정점의 위치
		XMFLOAT3 normal; //법선벡터
		XMFLOAT3 tan; //탄젠트 벡터
		XMFLOAT2 tex; //텍스쳐 좌표
		
		BasicVertex() :pos(0), normal(0), tan(0), tex(0) {}
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
	XMCOLOR color; //기본색상
	XMMATRIX world; //월드좌표
};