#pragma once

#include <D3D11.h>
#include <vector>
#include <d3dUtil.h>
#include "BufferResource.h"


using namespace std;

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


class Mesh
{
private:
	
	vector<Subset> subsets;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3D11Buffer* InstanceBuffer;
public:
	//반직선 검출등에 사용
	vector<MyVertex::BasicVertex> vertices;
	vector<UINT> indices;
	vector<InstancingData> InstancingDatas;
	
	//생성자
	Mesh() : mVB(0), mIB(0) {}
	~Mesh() { ReleaseCOM(mVB); ReleaseCOM(mIB); ReleaseCOM(InstanceBuffer); }
	
	void SetVertices(vector<MyVertex::BasicVertex>& vertexSrc);
	void SetIndices(vector<UINT>& indexSrc);
	

	void Draw(ID3D11DeviceContext* context, UINT subsetIdx);

	template <typename VertexType>
	void SetVB(ID3D11Device* device);
	void SetIB(ID3D11Device* device);
	void SetInstanceBuffer(ID3D11Device* device, int bufferSize);

	ID3D11Buffer* GetVB() { return mVB; }
	ID3D11Buffer* GetIB() { return mVB; }
	ID3D11Buffer* GetInstanceBuffer() { return InstanceBuffer; }
	int GetSubsetLength() { return subsets.size(); }

};

//정점버퍼 생성
template<typename VertexType>
inline void Mesh::SetVB(ID3D11Device * device)
{
	ReleaseCOM(mVB);
	
	//bufferDesc 작성
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(VertexType) * vertices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //구조적버퍼에 저장된 원소 하나의 크기, 구조적버퍼 사용할때 필요
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &vertices;

	HR(device->CreateBuffer(&desc, &subRes, &mVB));
}
