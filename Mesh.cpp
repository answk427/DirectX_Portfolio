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

//�ε������� ����
void Mesh::SetIB(ID3D11Device * device)
{
	ReleaseCOM(mIB);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * indices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //���������ۿ� ����� ���� �ϳ��� ũ��, ���������� ����Ҷ� �ʿ�
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &indices;

	HR(device->CreateBuffer(&desc, &subRes, &mIB));
}

void Mesh::SetInstanceBuffer(ID3D11Device * device, int bufferSize)
{
	ReleaseCOM(InstanceBuffer);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(InstancingData) * bufferSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //���� ���� ����
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; 
	desc.Usage = D3D11_USAGE_DYNAMIC;
	
	//������ instancingData�� ������ ���� �����̹Ƿ� subresource�� null
	HR(device->CreateBuffer(&desc, 0, &InstanceBuffer));
}
