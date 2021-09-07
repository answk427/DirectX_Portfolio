#include "Mesh.h"


void Mesh::SetVertices(vector<MyVertex::BasicVertex>& vertexSrc)
{
	//vertices를 빈 벡터로 만듬
	vector<MyVertex::BasicVertex>().swap(vertices);
	//매개변수 벡터와 교환
	vertices.swap(vertexSrc);
}

void Mesh::SetIndices(vector<UINT>& indexSrc)
{
	//Indices를 빈 벡터로 만듬
	vector<UINT>().swap(indices);
	//매개변수 벡터와 교환
	indices.swap(indexSrc);
}

void Mesh::SetSubset(vector<Subset>& subsetSrc)
{
	//subsets를 빈 벡터로 만듬
	vector<Subset>().swap(subsets);
	//매개변수 벡터와 교환
	subsets.swap(subsetSrc);
}

void Mesh::Draw(ID3D11DeviceContext * context, UINT subsetIdx)
{
	context->DrawIndexed(subsets[subsetIdx].IndexCount
		, subsets[subsetIdx].IndexStart
		, subsets[subsetIdx].VertexStart);

}

//인덱스버퍼 생성
void Mesh::SetIB(ID3D11Device * device)
{
	ReleaseCOM(mIB);

	//bufferDesc 작성
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * indices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //구조적버퍼에 저장된 원소 하나의 크기, 구조적버퍼 사용할때 필요
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &indices;

	HR(device->CreateBuffer(&desc, &subRes, &mIB));
}

void Mesh::SetInstanceBuffer(ID3D11Device * device, int bufferSize)
{
	ReleaseCOM(InstanceBuffer);

	//bufferDesc 작성
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(InstancingData) * bufferSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //쓰기 가능 버퍼
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	//매핑후 instancingData의 내용을 직접 쓸것이므로 subresource는 null
	HR(device->CreateBuffer(&desc, 0, &InstanceBuffer));
}
