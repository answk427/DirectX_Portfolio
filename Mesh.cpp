#include "Mesh.h"


void Mesh::Draw(ID3D11DeviceContext * context)
{
	for (int i = 0; i < subsets.size(); i++)
	{
		context->DrawIndexed(subsets[i].IndexCount
			, subsets[i].IndexStart
			, subsets[i].VertexStart);
	}
	
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
