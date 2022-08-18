#pragma once

#include <D3D11.h>
#include <vector>
#include <d3dUtil.h>
#include "BufferResource.h"
#include <UtilFunctions.h>
#include <unordered_map>

#include "SkinningComputeShader.h"
#define MAX_INSTSANCING 1000




class Mesh
{
private:
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3D11Buffer* m_InstanceBasicBuffer;
	ID3D11Buffer* m_InstanceMatrixBuffer;
	
	ID3D11Buffer* m_skinnedDataBuffer;
	XNA::AxisAlignedBox m_AABB;
	AABB_MaxMin m_Aabb_MaxMin;
	bool m_instancing; //�ν��Ͻ� ������ ���θ� ��Ÿ���� ����
	

private:
	UINT vertexBufferCount;
	//�ش� ������ŭ �ν��Ͻ������� ũ�⸦ �����Ѵ�.
	UINT m_instanceBufferSize;
public:
	std::unique_ptr<SkinningInstancingComputeShader> m_instancingComputeShader;

	void InitInstancingComputeShader(const std::wstring& filename, ID3D11Device* device);
	//�����ʱ�ȭ �Լ���
	//�������� ����
	void InitVB(ID3D11Device* device);
	void InitIB(ID3D11Device* device);

	//���� vertex ����ü�� �̿��� ���۸� �ʱ�ȭ�ϴ� �Լ�
	template <typename vertex>
	void InitVB(ID3D11Device* device, std::vector<vertex>& vertices);
	template <typename skinData>
	void InitSkinnedVB(ID3D11Device* device, std::vector<skinData>& skinDatas);

	void InitInstanceBuffer(ID3D11Device* device, UINT bufferSize);
	//���� Ŭ������ m_instanceBufferSize ��������� ���ۻ����� ����
	void InitInstanceBuffer(ID3D11Device* device);

	//���� vertex ����ü�� �̿��� ���Ⱑ�� ���۸� �ʱ�ȭ�ϴ� �Լ�
	template <typename vertex>
	void InitWritableVB(ID3D11Device* device, UINT bufferSize);
	void InitWritableIB(ID3D11Device* device, UINT bufferSize);

private:
	//ObjectLoader���� �Ž��������� ���޹޴� �Լ�
	void SetVertices(std::vector<MyVertex::BasicVertex>& vertexSrc);
	void SetIndices(std::vector<UINT>& indexSrc);
	void SetSubsets(std::vector<MyVertex::Subset>& subsetSrc);


public:
	std::string id;
	std::vector<MyVertex::BasicVertex> vertices;
	std::vector<UINT> indices;
	std::vector<MyVertex::Subset> subsets;
	//���� instancingData�� ��� �ִ� std::vector		
	std::vector<InstancingData*> InstancingDatas;
	//�̹� ���������� ����� instancingData�� index.
	std::vector<UINT> enableInstancingIndexes;


#pragma region InstancingTextureBuffer
	//Texture �迭���� ����� �ؽ��ĵ��� ���
	std::vector<std::wstring>* textureNames{ 0 };
	//Subset���� ����� Texture�迭��
	std::vector<ID3D11ShaderResourceView*> textureArrays;
	//Texture �迭�� ũ�Ⱑ �޶����ų� �ٽ� �������� ��� ����ϴ� �Լ�
	void CreateTextureArrayResourceView(ID3D11Device* device, ID3D11DeviceContext* context);
	//Texture �迭�� �κи� �����ϴ� �Լ�
	void ModifyTextureArraySubResource(ID3D11Device* device, ID3D11DeviceContext* context,
		UINT rendererIdx, UINT subsetIdx, const std::wstring& textureName);
#pragma endregion


	//vertex, index���� �Է������⿡ ���ε�.
	void SetVB(ID3D11DeviceContext* context);
	void SetInstanceVB(ID3D11DeviceContext* context);
	void SetSkinnedVB(ID3D11DeviceContext* context);
	void SetInstanceSkinnedVB(ID3D11DeviceContext* context);

	void SetIB(ID3D11DeviceContext* context);
	void SetID(std::string id) { this->id = id; }

	//aabb ���� �Լ�
	void SetAABB_MaxMin(XMFLOAT3& maxV, XMFLOAT3& minV);
	AABB_MaxMin& GetAABB_MaxMin() { return m_Aabb_MaxMin; }
	XNA::AxisAlignedBox& GetAABB() { return m_AABB; }
	const XNA::AxisAlignedBox& GetAABB() const { return m_AABB; }

	//instancing ���� �Լ�
	void SetInstanceBufferSize(UINT n)
	{
		m_instanceBufferSize = n;
		InstancingDatas.reserve(m_instanceBufferSize);
		enableInstancingIndexes.reserve(m_instanceBufferSize);
	}
	bool GetInstancing() { return m_instancing; }
	bool SetInstancing(bool instancing) { return m_instancing = instancing; }
	void InstancingUpdate(ID3D11DeviceContext* context);
	void InstancingBasicUpdate(ID3D11DeviceContext* context);

public:
	//������
	Mesh() : mVB(0), mIB(0), m_InstanceBasicBuffer(0), m_skinnedDataBuffer(0),vertexBufferCount(0),
		id("temp"), m_instanceBufferSize(MAX_INSTSANCING), m_instancing(0), m_InstanceMatrixBuffer(0)
	{
		SetInstanceBufferSize(MAX_INSTSANCING);
		SetAABB_MaxMin(XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT3(-2.0f, -2.0f, -2.0f));
	}
	Mesh(const std::string& id) : Mesh()
	{
		this->id = id;
	}
	
	~Mesh();
	//���������
	Mesh(const Mesh& other);
	//�̵�������
	Mesh(Mesh&& other);
	//���Կ�����
	Mesh& operator=(const Mesh& other);


public:
	void Init(ID3D11Device* device,
		std::vector<MyVertex::BasicVertex>& vertexSrc,
		std::vector<UINT>& indexSrc,
		std::vector<MyVertex::Subset>& subsetSrc);

	void Init(std::vector<MyVertex::BasicVertex>& vertexSrc,
		std::vector<UINT>& indexSrc,
		std::vector<MyVertex::Subset>& subsetSrc);

	//�⺻ Subset 1���ΰɷ� ����
	void Init(std::vector<MyVertex::BasicVertex>& vertexSrc,
		std::vector<UINT>& indexSrc);

	void InitBuffers(ID3D11Device* device);
	
	template <typename vertexType>
	void InitSingleSubset(std::vector<vertexType>& vertices);

public:
	void Draw(ID3D11DeviceContext* context, UINT subsetIdx);
	void InstanceDraw(ID3D11DeviceContext* context, UINT subsetIdx);

	ID3D11Buffer* GetVB() { return mVB; }
	ID3D11Buffer* GetIB() { return mVB; }
	ID3D11Buffer* GetInstanceBuffer() { return m_InstanceBasicBuffer; }

	int GetSubsetLength() { return subsets.size(); }
	const std::vector<MyVertex::Subset>& GetSubsets() { return subsets; }
	const std::vector<MyVertex::BasicVertex>& GetVertices() { return vertices; }
	const std::vector<UINT>& GetIndices() { return indices; }

public:
	bool operator==(const Mesh& mesh) { return id < mesh.id; }
	bool operator==(const std::string& name) { return id == name; }

};

template<typename vertex>
inline void Mesh::InitVB(ID3D11Device * device, std::vector<vertex>& vertices)
{
	if (vertices.empty())
		return;
	ReleaseCOM(mVB);

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(vertex) * vertices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //���������ۿ� ����� ���� �ϳ��� ũ��, ���������� ����Ҷ� �ʿ�
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &vertices[0];

	HR(device->CreateBuffer(&desc, &subRes, &mVB));

	vertexBufferCount++;
}

template<typename skinData>
inline void Mesh::InitSkinnedVB(ID3D11Device * device, std::vector<skinData>& skinDatas)
{
	ReleaseCOM(m_skinnedDataBuffer);	

	//bufferDesc �ۼ�
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(skinData) * skinDatas.size();
	desc.CPUAccessFlags = 0; 
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &skinDatas[0];

	HR(device->CreateBuffer(&desc, &subRes, &m_skinnedDataBuffer));

}

template<typename vertex>
inline void Mesh::InitWritableVB(ID3D11Device * device, UINT bufferSize)
{
	ReleaseCOM(mVB);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(vertex) * bufferSize;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(device->CreateBuffer(&vbd, 0, &mVB));

	vertexBufferCount++;
}

template<typename vertexType>
inline void Mesh::InitSingleSubset(std::vector<vertexType>& vertices)
{
	MyVertex::Subset subset;
	subset.IndexCount = indices.size();
	subset.IndexStart = 0;
	subset.VertexCount = vertices.size();
	subset.VertexStart = 0;
	subset.materialNum = 0;

	
	std::vector<MyVertex::Subset> tempSubsets(1, subset);
	
	SetSubsets(tempSubsets);
}


