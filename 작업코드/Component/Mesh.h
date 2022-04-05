#pragma once

#include <D3D11.h>
#include <vector>
#include <d3dUtil.h>
#include "BufferResource.h"
#include <UtilFunctions.h>
#include <unordered_map>

using namespace std;

#define MAX_INSTSANCING 1000

//��������, �ε������۸� 1���� ����� ���
//(���� �ϳ��� ���� �޽��� ����,�ε����� ���Ƴִ� ��)
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
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3D11Buffer* m_InstanceBuffer;
	XNA::AxisAlignedBox m_AABB;
	AABB_MaxMin m_Aabb_MaxMin;
	bool m_instancing; //�ν��Ͻ� ������ ���θ� ��Ÿ���� ����


private:
	UINT vertexBufferCount;
	//�ش� ������ŭ �ν��Ͻ������� ũ�⸦ �����Ѵ�.
	UINT m_instanceBufferSize;
public:
	//�����ʱ�ȭ �Լ���
	//�������� ����
	void InitVB(ID3D11Device* device);
	void InitIB(ID3D11Device* device);

	//���� vertex ����ü�� �̿��� ���۸� �ʱ�ȭ�ϴ� �Լ�
	template <typename vertex>
	void InitVB(ID3D11Device* device, std::vector<vertex> vertices);
	
	void InitInstanceBuffer(ID3D11Device* device, UINT bufferSize);
	//���� Ŭ������ m_instanceBufferSize ��������� ���ۻ����� ����
	void InitInstanceBuffer(ID3D11Device* device);

	//���� vertex ����ü�� �̿��� ���Ⱑ�� ���۸� �ʱ�ȭ�ϴ� �Լ�
	template <typename vertex>
	void InitWritableVB(ID3D11Device* device, UINT bufferSize);
	void InitWritableIB(ID3D11Device* device, UINT bufferSize);

private:
	//ObjectLoader���� �Ž��������� ���޹޴� �Լ�
	void SetVertices(vector<MyVertex::BasicVertex>& vertexSrc);
	void SetIndices(vector<UINT>& indexSrc);
	void SetSubsets(vector<Subset>& subsetSrc);

	
public:
	std::string id;

	//������ ���� ����� ������ ����		
	vector<MyVertex::BasicVertex> vertices;
	vector<UINT> indices;
	vector<Subset> subsets;
	//���� instancingData�� ��� �ִ� vector
	std::vector<InstancingData*> InstancingDatas;
	//�̹� ���������� ����� instancingData�� index.
	vector<UINT> enableInstancingIndexes;

#pragma region InstancingTextureBuffer
	//Texture �迭���� ����� �ؽ��ĵ��� ���
	vector<std::wstring>* textureNames{0};
	//Subset���� ����� Texture�迭��
	vector<ID3D11ShaderResourceView*> textureArrays;
	//Texture �迭�� ũ�Ⱑ �޶����ų� �ٽ� �������� ��� ����ϴ� �Լ�
	void CreateTextureArrayResourceView(ID3D11Device* device, ID3D11DeviceContext* context);
	//Texture �迭�� �κи� �����ϴ� �Լ�
	void ModifyTextureArraySubResource(ID3D11Device* device, ID3D11DeviceContext* context, 
		UINT rendererIdx, UINT subsetIdx, const std::wstring& textureName);
#pragma endregion

	
	
	//vertex, index���� �Է������⿡ ���ε�.
	void SetVB(ID3D11DeviceContext* context);
	void SetInstanceVB(ID3D11DeviceContext* context);
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

public:
	//������
	Mesh(const std::string& id) : mVB(0), mIB(0), m_InstanceBuffer(0), vertexBufferCount(0),
		id(id), m_instanceBufferSize(MAX_INSTSANCING), m_instancing(0)
	{	SetInstanceBufferSize(MAX_INSTSANCING);	}
	Mesh() : mVB(0), mIB(0), m_InstanceBuffer(0), vertexBufferCount(0), id("temp"), m_instanceBufferSize(MAX_INSTSANCING), m_instancing(0)
	{	SetInstanceBufferSize(MAX_INSTSANCING);	}
	~Mesh(); 
	//���������
	Mesh(const Mesh& other);
	//�̵�������
	Mesh(Mesh&& other);
	//���Կ�����
	Mesh& operator=(const Mesh& other);

	 
public:
	void Init(ID3D11Device* device,
		vector<MyVertex::BasicVertex>& vertexSrc,
		vector<UINT>& indexSrc,
		vector<Subset>& subsetSrc);
	
	void Init(vector<MyVertex::BasicVertex>& vertexSrc,
		vector<UINT>& indexSrc,
		vector<Subset>& subsetSrc);

	//�⺻ Subset 1���ΰɷ� ����
	void Init(vector<MyVertex::BasicVertex>& vertexSrc,
		vector<UINT>& indexSrc);
	
	void InitBuffers(ID3D11Device* device);

public:
	void Draw(ID3D11DeviceContext* context, UINT subsetIdx);
	void InstanceDraw(ID3D11DeviceContext* context, UINT subsetIdx);

	ID3D11Buffer* GetVB() { return mVB; }
	ID3D11Buffer* GetIB() { return mVB; }
	ID3D11Buffer* GetInstanceBuffer() { return m_InstanceBuffer; }

	int GetSubsetLength() { return subsets.size(); }
	const std::vector<Subset>& GetSubsets(){ return subsets; }
	const std::vector<MyVertex::BasicVertex>& GetVertices() { return vertices; }
	const std::vector<UINT>& GetIndices() { return indices; }

public:
	bool operator==(const Mesh& mesh) { return id < mesh.id; }
	bool operator==(const std::string& name) { return id == name; }
};

template<typename vertex>
inline void Mesh::InitVB(ID3D11Device * device, std::vector<vertex> vertices)
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
