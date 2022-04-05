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

void Mesh::SetSubsets(vector<Subset>& subsetSrc)
{
	//subsets�� �� ���ͷ� ����
	vector<Subset>().swap(subsets);
	//�Ű����� ���Ϳ� ��ȯ
	subsets.swap(subsetSrc);

	//subset �����ŭ �����Ǿ� �ϴ� ���״Ͼ�� �ʱ�ȭ
	if (textureNames != nullptr)
		delete[] textureNames;
	textureNames = new vector<std::wstring>[subsets.size()];
	textureArrays.assign(subsets.size(), 0);

}

void Mesh::CreateTextureArrayResourceView(ID3D11Device* device, ID3D11DeviceContext* context)
{

	for (int i = 0; i < textureArrays.size(); ++i)
	{
		ReleaseCOM(textureArrays[i]);
		textureArrays[i] = 
			d3dHelper::CreateTexture2DArraySRV(device, context, textureNames[i], DXGI_FORMAT_R8G8B8A8_UNORM);
	}
}

void Mesh::ModifyTextureArraySubResource(ID3D11Device* device, ID3D11DeviceContext* context,
	UINT rendererIdx, UINT subsetIdx, const std::wstring& textureName)
{
	textureNames[subsetIdx][rendererIdx] = textureName;
	//�ؽ��Ĺ迭�� ���»��¸� ���� �����.
	if (textureArrays[subsetIdx] == nullptr)
	{
		textureArrays[subsetIdx] = d3dHelper::CreateTexture2DArraySRV(device, context,
			textureNames[subsetIdx], DXGI_FORMAT_R8G8B8A8_UNORM);
	}
	else //�ؽ��Ĺ迭�� �ִٸ� �κи� ����
	{
		if (d3dHelper::ModifyTexture2DArraySRV(device, context, textureArrays[subsetIdx], rendererIdx,
			textureNames[subsetIdx][rendererIdx], DXGI_FORMAT_R8G8B8A8_UNORM))
		{
		}//���������� �ڵ�
	}
}

void Mesh::SetVB(ID3D11DeviceContext * context)
{
	UINT stride[1] = { sizeof(vertices[0]) };
	UINT offset[1] = { 0 };
	context->IASetVertexBuffers(0, 1, &mVB, stride, offset);
}

void Mesh::SetInstanceVB(ID3D11DeviceContext * context)
{
	UINT stride[2] = { sizeof(vertices[0]), sizeof(InstancingData) };
	UINT offset[2] = { 0 ,0};
	ID3D11Buffer* vbs[2] = { mVB,m_InstanceBuffer };
	context->IASetVertexBuffers(0, 2, vbs, stride, offset);
}

void Mesh::SetIB(ID3D11DeviceContext * context)
{
	context->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::SetAABB_MaxMin(XMFLOAT3 & maxV, XMFLOAT3 & minV)
{
	m_Aabb_MaxMin.m_max = maxV;
	m_Aabb_MaxMin.m_min = minV;
	ConvertAABBtypeToCenter(m_AABB, maxV, minV);
}

void Mesh::InstancingUpdate(ID3D11DeviceContext* context)
{
	if (m_InstanceBuffer == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(m_InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	
	InstancingData* dataView = reinterpret_cast<InstancingData*>(mappedData.pData);

	for (int i = 0; i < enableInstancingIndexes.size(); ++i)
	{
		if(InstancingDatas[enableInstancingIndexes[i]] != nullptr)
			dataView[i] = *InstancingDatas[enableInstancingIndexes[i]];
	}

	context->Unmap(m_InstanceBuffer, 0);
}

Mesh::~Mesh()
{
	for (int i = 0; i < InstancingDatas.size(); ++i)
	{
		if (InstancingDatas[i] != nullptr)
			delete InstancingDatas[i];
	}
	if (textureNames != nullptr)
		delete[] textureNames;
	ReleaseCOM(mVB); ReleaseCOM(mIB); ReleaseCOM(m_InstanceBuffer);
}
Mesh::Mesh(const Mesh & other) : mVB(0), mIB(0), m_InstanceBuffer(0), vertexBufferCount(0), id(other.id), m_instancing(0)
{
	std::vector<Subset> tempSubsets = other.subsets;
	SetSubsets(tempSubsets);
	vertices = other.vertices;
	InstancingDatas = other.InstancingDatas;
	indices = other.indices;
	//boundingBox
	m_AABB = other.m_AABB;
	m_Aabb_MaxMin = other.m_Aabb_MaxMin;
	//instanceBuffer
	enableInstancingIndexes = other.enableInstancingIndexes;
	m_InstanceBuffer = other.m_InstanceBuffer;
	SetInstanceBufferSize(MAX_INSTSANCING);
}

Mesh::Mesh(Mesh && other) : mVB(0), mIB(0), m_InstanceBuffer(0), vertexBufferCount(0), id(other.id), m_instancing(0)
{
	std::vector<Subset> tempSubsets = other.subsets;
	SetSubsets(tempSubsets);

	vertices.swap(other.vertices);
	InstancingDatas.swap(other.InstancingDatas);
	indices.swap(other.indices);
	//boundingBox
	m_AABB = other.m_AABB;
	m_Aabb_MaxMin = other.m_Aabb_MaxMin;
	//instanceBuffer
	enableInstancingIndexes = other.enableInstancingIndexes;
	m_InstanceBuffer = other.m_InstanceBuffer;
	SetInstanceBufferSize(MAX_INSTSANCING);
}

Mesh & Mesh::operator=(const Mesh & other)
{
	std::vector<Subset> tempSubsets = other.subsets;
	SetSubsets(tempSubsets);

	vertices = other.vertices;
	InstancingDatas = other.InstancingDatas;
	indices = other.indices;
	enableInstancingIndexes = other.enableInstancingIndexes;
	id = other.id;
	//boundingBox
	m_AABB = other.m_AABB;
	m_Aabb_MaxMin = other.m_Aabb_MaxMin;
	m_instancing = other.m_instancing;
	m_instanceBufferSize = other.m_instanceBufferSize;
	m_InstanceBuffer = other.m_InstanceBuffer;
	return *this;
}

void Mesh::Init(ID3D11Device * device, vector<MyVertex::BasicVertex>& vertexSrc, vector<UINT>& indexSrc, vector<Subset>& subsetSrc)
{
	SetVertices(vertexSrc);
	SetIndices(indexSrc);
	SetSubsets(subsetSrc);
	InitBuffers(device);
}

void Mesh::Init(vector<MyVertex::BasicVertex>& vertexSrc, vector<UINT>& indexSrc, vector<Subset>& subsetSrc)
{
	SetVertices(vertexSrc);
	SetIndices(indexSrc);
	SetSubsets(subsetSrc);
}

void Mesh::Init(vector<MyVertex::BasicVertex>& vertexSrc, vector<UINT>& indexSrc)
{
	SetVertices(vertexSrc);
	SetIndices(indexSrc);
	std::vector<Subset> subsets(1);
	subsets[0].VertexCount = vertices.size();
	subsets[0].IndexCount = indices.size();
	subsets[0].VertexStart = 0;
	subsets[0].IndexStart = 0;
	subsets[0].materialNum = 1;
	
	SetSubsets(subsets);
}

void Mesh::InitBuffers(ID3D11Device * device)
{
	InitVB(device);
	InitIB(device);
	InitInstanceBuffer(device);
}

void Mesh::Draw(ID3D11DeviceContext * context, UINT subsetIdx)
{
	if (!m_instancing)
	{
		context->DrawIndexed(subsets[subsetIdx].IndexCount
			, subsets[subsetIdx].IndexStart
			, subsets[subsetIdx].VertexStart);
	}
	else
	{
		//�ν��Ͻ� ������
		context->DrawIndexedInstanced(subsets[subsetIdx].IndexCount
			, enableInstancingIndexes.size()
			, subsets[subsetIdx].IndexStart
			, subsets[subsetIdx].VertexStart
			, 0);
	}
	
}

void Mesh::InstanceDraw(ID3D11DeviceContext * context, UINT subsetIdx)
{
	context->DrawIndexedInstanced(subsets[subsetIdx].IndexCount
		, enableInstancingIndexes.size()
		, subsets[subsetIdx].IndexStart
		, subsets[subsetIdx].VertexStart
		, 0);
}

void Mesh::InitVB(ID3D11Device * device)
{
	ReleaseCOM(mVB);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(vertices[0]) * vertices.size();	
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //���������ۿ� ����� ���� �ϳ��� ũ��, ���������� ����Ҷ� �ʿ�
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &vertices[0];

	HR(device->CreateBuffer(&desc, &subRes, &mVB));

	vertexBufferCount++;
}

//�ε������� ����
void Mesh::InitIB(ID3D11Device * device)
{
	if (indices.empty())
		return;
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
	subRes.pSysMem = &indices[0];

	HR(device->CreateBuffer(&desc, &subRes, &mIB));
}

void Mesh::InitWritableIB(ID3D11Device * device, UINT bufferSize)
{
	ReleaseCOM(mIB);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * bufferSize;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //���������ۿ� ����� ���� �ϳ��� ũ��, ���������� ����Ҷ� �ʿ�
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &indices[0];

	HR(device->CreateBuffer(&desc, &subRes, &mIB));
}

void Mesh::InitInstanceBuffer(ID3D11Device * device, UINT bufferSize)
{
	ReleaseCOM(m_InstanceBuffer);
	SetInstanceBufferSize(bufferSize);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(InstancingData) * bufferSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //���� ���� ����
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	//������ instancingData�� ������ ���� �����̹Ƿ� subresource�� null
	HR(device->CreateBuffer(&desc, 0, &m_InstanceBuffer));
}

void Mesh::InitInstanceBuffer(ID3D11Device * device)
{
	ReleaseCOM(m_InstanceBuffer);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(InstancingData) * m_instanceBufferSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //���� ���� ����
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	//������ instancingData�� ������ ���� �����̹Ƿ� subresource�� null
	HR(device->CreateBuffer(&desc, 0, &m_InstanceBuffer));
}


