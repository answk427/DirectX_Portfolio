#pragma once

#include <D3D11.h>
#include <vector>
#include <d3dUtil.h>
#include "BufferResource.h"
#include <UtilFunctions.h>

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
	vector<Subset> subsets;
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
	
	//���� vertex ����ü�� �̿��� ���۸� �ʱ�ȭ�ϴ� �Լ�
	template <typename vertex>
	void InitVB(ID3D11Device* device, std::vector<vertex> vertices);

	void InitIB(ID3D11Device* device);
	void InitInstanceBuffer(ID3D11Device* device, UINT bufferSize);
	//���� Ŭ������ m_instanceBufferSize ��������� ���ۻ����� ����
	void InitInstanceBuffer(ID3D11Device* device);
	void InitWritableVB(ID3D11Device* device);

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
	//���� instancingData�� ��� �ִ� vector
	vector<InstancingData> InstancingDatas;
	//�̹� ���������� ����� instancingData�� index.
	vector<UINT> enableInstancingIndexes;

	
	
	//vertex, index���� �Է������⿡ ���ε�.
	void SetVB(ID3D11DeviceContext* context);
	void SetInstanceVB(ID3D11DeviceContext* context);
	void SetIB(ID3D11DeviceContext* context);
	void SetID(std::string id) { this->id = id; }
	
	//aabb ���� �Լ�
	void SetAABB_MaxMin(XMFLOAT3& maxV, XMFLOAT3& minV);
	AABB_MaxMin& GetAABB_MaxMin() { return m_Aabb_MaxMin; }
	XNA::AxisAlignedBox& GetAABB() { return m_AABB; }

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
	~Mesh() {
		ID3D11Buffer* VB = mVB;
		ID3D11Buffer* IB = mIB;
		ID3D11Buffer* inst = m_InstanceBuffer;
		ReleaseCOM(mVB); ReleaseCOM(mIB); ReleaseCOM(m_InstanceBuffer); 
	}
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
