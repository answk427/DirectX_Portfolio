#pragma once

#include <D3D11.h>
#include <vector>

using namespace std;

//정점버퍼, 인덱스버퍼를 1개씩 사용할 경우 
class Subset 
{
public:
	UINT VertexCount;
	UINT VertexStart;
	UINT FaceCount;
	UINT FaceStart;
	UINT id;
	
	Subset() : VertexCount(0), VertexStart(0),
		FaceCount(0), FaceStart(0), id(0) {}
};


class Mesh
{
private:
	vector<Subset> subsets;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
public:
	Mesh() : mVB(0), mIB(0) {}
	
	void Draw(ID3D11DeviceContext* context); 

	template <typename VertexType>
	void SetVB(ID3D11Device* device, const vector<VertexType>& vertices);

	void SetIB(ID3D11Device* device, const vector<UINT>& indices);

};

//정점버퍼 생성
template<typename VertexType>
inline void Mesh::SetVB(ID3D11Device * device, const vector<VertexType>& vertices)
{
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

	device->CreateBuffer(&desc, &subRes, &mVB);
}
