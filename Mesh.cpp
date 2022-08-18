#include "Mesh.h"


void Mesh::SetVertices(std::vector<MyVertex::BasicVertex>& vertexSrc)
{
	//vertices�� �� ���ͷ� ����
	std::vector<MyVertex::BasicVertex>().swap(vertices);
	//�Ű����� ���Ϳ� ��ȯ
	vertices.swap(vertexSrc);
}

void Mesh::SetIndices(std::vector<UINT>& indexSrc)
{
	//Indices�� �� ���ͷ� ����
	std::vector<UINT>().swap(indices);
	//�Ű����� ���Ϳ� ��ȯ
	indices.swap(indexSrc);
}

void Mesh::SetSubsets(std::vector<MyVertex::Subset>& subsetSrc)
{
	//subsets�� �� ���ͷ� ����
	std::vector<MyVertex::Subset>().swap(subsets);
	//�Ű����� ���Ϳ� ��ȯ
	subsets.swap(subsetSrc);

	//subset �����ŭ �����Ǿ� �ϴ� ���״Ͼ�� �ʱ�ȭ
	if (textureNames != nullptr)
		delete[] textureNames;
	textureNames = new std::vector<std::wstring>[subsets.size()];
	//textureArrays.assign(subsets.size(), 0);
	textureArrays.push_back(nullptr);

	textureArrays.resize(subsets.size());
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
	UINT stride[3] = { sizeof(vertices[0]), sizeof(InstancingMatrixData), sizeof(InstancingBasicData) };
	UINT offset[3] = { 0 ,0, 0};
	ID3D11Buffer* vbs[3] = { mVB,m_InstanceMatrixBuffer,m_InstanceBasicBuffer };
	context->IASetVertexBuffers(0, 3, vbs, stride, offset);
}

void Mesh::SetSkinnedVB(ID3D11DeviceContext * context)
{
	UINT stride[2] = { sizeof(vertices[0]), sizeof(MyVertex::SkinnedData) };
	UINT offset[2] = { 0 ,0 };
	ID3D11Buffer* vbs[2] = { mVB, m_skinnedDataBuffer};
	context->IASetVertexBuffers(0, 2, vbs, stride, offset);
}

void Mesh::SetInstanceSkinnedVB(ID3D11DeviceContext * context)
{
	UINT stride[1] = { sizeof(InstancingBasicData)};
	UINT offset[1] = { 0 };
	
	context->IASetVertexBuffers(0,1, &m_InstanceBasicBuffer, stride, offset);
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
	if (m_InstanceBasicBuffer == nullptr || m_InstanceMatrixBuffer == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	D3D11_MAPPED_SUBRESOURCE matrixMappedData;

	context->Map(m_InstanceBasicBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	context->Map(m_InstanceMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &matrixMappedData);

	
	InstancingBasicData* dataView = reinterpret_cast<InstancingBasicData*>(mappedData.pData);
	InstancingMatrixData* matrixDataView = reinterpret_cast<InstancingMatrixData*>(matrixMappedData.pData);

	for (int i = 0; i < enableInstancingIndexes.size(); ++i)
	{
		if (InstancingDatas[enableInstancingIndexes[i]] != nullptr)
		{
			matrixDataView[i].world = InstancingDatas[enableInstancingIndexes[i]]->world;
			matrixDataView[i].worldInvTranspose = InstancingDatas[enableInstancingIndexes[i]]->worldInvTranspose;
			dataView[i].color = InstancingDatas[enableInstancingIndexes[i]]->color;
			dataView[i].RendererIdx = InstancingDatas[enableInstancingIndexes[i]]->RendererIdx;
			
		}			
	}

	context->Unmap(m_InstanceBasicBuffer, 0);
	context->Unmap(m_InstanceMatrixBuffer, 0);
}

void Mesh::InstancingBasicUpdate(ID3D11DeviceContext * context)
{
	if (m_InstanceBasicBuffer == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	
	context->Map(m_InstanceBasicBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		
	InstancingBasicData* dataView = reinterpret_cast<InstancingBasicData*>(mappedData.pData);
	
	for (int i = 0; i < enableInstancingIndexes.size(); ++i)
	{
		if (InstancingDatas[enableInstancingIndexes[i]] != nullptr)
		{
			dataView[i].color = InstancingDatas[enableInstancingIndexes[i]]->color;
			dataView[i].RendererIdx = InstancingDatas[enableInstancingIndexes[i]]->RendererIdx;

		}
	}

	context->Unmap(m_InstanceBasicBuffer, 0);
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
	ReleaseCOM(mVB); ReleaseCOM(mIB); ReleaseCOM(m_InstanceBasicBuffer);
	ReleaseCOM(m_skinnedDataBuffer);
	ReleaseCOM(m_InstanceMatrixBuffer);
}
Mesh::Mesh(const Mesh & other) : Mesh(other.id)
{
	std::vector<MyVertex::Subset> tempSubsets = other.subsets;
	SetSubsets(tempSubsets);
	vertices = other.vertices;
	InstancingDatas = other.InstancingDatas;
	indices = other.indices;
	//boundingBox
	m_AABB = other.m_AABB;
	m_Aabb_MaxMin = other.m_Aabb_MaxMin;
	//instanceBuffer
	enableInstancingIndexes = other.enableInstancingIndexes;
	m_InstanceBasicBuffer = other.m_InstanceBasicBuffer;
	SetInstanceBufferSize(MAX_INSTSANCING);
}

Mesh::Mesh(Mesh && other) : Mesh(other.id)
{
	std::vector<MyVertex::Subset> tempSubsets = other.subsets;
	SetSubsets(tempSubsets);

	vertices.swap(other.vertices);
	InstancingDatas.swap(other.InstancingDatas);
	indices.swap(other.indices);
	//boundingBox
	m_AABB = other.m_AABB;
	m_Aabb_MaxMin = other.m_Aabb_MaxMin;
	//instanceBuffer
	enableInstancingIndexes = other.enableInstancingIndexes;
	m_InstanceBasicBuffer = other.m_InstanceBasicBuffer;
	SetInstanceBufferSize(MAX_INSTSANCING);
}

Mesh & Mesh::operator=(const Mesh & other)
{
	std::vector<MyVertex::Subset> tempSubsets = other.subsets;
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
	m_InstanceBasicBuffer = other.m_InstanceBasicBuffer;
	return *this;
}

void Mesh::Init(ID3D11Device * device, std::vector<MyVertex::BasicVertex>& vertexSrc, std::vector<UINT>& indexSrc, std::vector<MyVertex::Subset>& subsetSrc)
{
	SetVertices(vertexSrc);
	SetIndices(indexSrc);
	SetSubsets(subsetSrc);
	InitBuffers(device);
}

void Mesh::Init(std::vector<MyVertex::BasicVertex>& vertexSrc, std::vector<UINT>& indexSrc, std::vector<MyVertex::Subset>& subsetSrc)
{
	SetVertices(vertexSrc);
	SetIndices(indexSrc);
	SetSubsets(subsetSrc);
}

void Mesh::Init(std::vector<MyVertex::BasicVertex>& vertexSrc, std::vector<UINT>& indexSrc)
{
	SetVertices(vertexSrc);
	SetIndices(indexSrc);
	std::vector<MyVertex::Subset> subsets(1);
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

void Mesh::InitInstancingComputeShader(const std::wstring & filename, ID3D11Device * device)
{
	if (m_instancingComputeShader)
		return;
	m_instancingComputeShader = std::make_unique
		<SkinningInstancingComputeShader>(filename, device);
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
	ReleaseCOM(m_InstanceBasicBuffer);
	ReleaseCOM(m_InstanceMatrixBuffer);

	SetInstanceBufferSize(bufferSize);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(InstancingBasicData) * bufferSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //���� ���� ����
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	//������ instancingData�� ������ ���� �����̹Ƿ� subresource�� null
	HR(device->CreateBuffer(&desc, 0, &m_InstanceBasicBuffer));

	desc.ByteWidth = sizeof(InstancingMatrixData) * bufferSize;
	HR(device->CreateBuffer(&desc, 0, &m_InstanceMatrixBuffer));
}

void Mesh::InitInstanceBuffer(ID3D11Device * device)
{
	ReleaseCOM(m_InstanceBasicBuffer);
	ReleaseCOM(m_InstanceMatrixBuffer);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(InstancingBasicData) * m_instanceBufferSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //���� ���� ����
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	//������ instancingData�� ������ ���� �����̹Ƿ� subresource�� null
	HR(device->CreateBuffer(&desc, 0, &m_InstanceBasicBuffer));

	desc.ByteWidth = sizeof(InstancingMatrixData) * m_instanceBufferSize;
	HR(device->CreateBuffer(&desc, 0, &m_InstanceMatrixBuffer));

}



