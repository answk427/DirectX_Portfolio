#include "Mesh.h"


void Mesh::SetVertices(vector<MyVertex::BasicVertex>& vertexSrc)
{
	//vertices�� �� ���ͷ� ����
	vector<MyVertex::BasicVertex>().swap(vertices);
	//�Ű����� ���Ϳ� ��ȯ
	vertices.swap(vertexSrc);
}

void Mesh::SetIndices(vector<UINT>& indexSrc)
{
	//Indices�� �� ���ͷ� ����
	vector<UINT>().swap(indices);
	//�Ű����� ���Ϳ� ��ȯ
	indices.swap(indexSrc);
}

void Mesh::SetSubset(vector<Subset>& subsetSrc)
{
	//subsets�� �� ���ͷ� ����
	vector<Subset>().swap(subsets);
	//�Ű����� ���Ϳ� ��ȯ
	subsets.swap(subsetSrc);
}

void Mesh::Draw(ID3D11DeviceContext * context, UINT subsetIdx)
{
	context->DrawIndexed(subsets[subsetIdx].IndexCount
		, subsets[subsetIdx].IndexStart
		, subsets[subsetIdx].VertexStart);

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
