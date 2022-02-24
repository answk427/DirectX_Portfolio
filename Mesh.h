#pragma once

#include <D3D11.h>
#include <vector>
#include <d3dUtil.h>
#include "BufferResource.h"


using namespace std;

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
	ID3D11Buffer* InstanceBuffer;

private:
	int vertexBufferCount;
private:
	//�����ʱ�ȭ �Լ���
	//�������� ����
	void InitVB(ID3D11Device* device);
	void InitIB(ID3D11Device* device);
	void InitInstanceBuffer(ID3D11Device* device, int bufferSize);

private:
	//ObjectLoader���� �Ž��������� ���޹޴� �Լ�
	void SetVertices(vector<MyVertex::BasicVertex>& vertexSrc);
	void SetIndices(vector<UINT>& indexSrc);
	void SetSubsets(vector<Subset>& subsetSrc);

	
public:
	std::string id;

	//������ ���� ���			
	vector<MyVertex::BasicVertex> vertices;
	vector<InstancingData> InstancingDatas;
	vector<UINT> indices;
	
	void SetVB(ID3D11DeviceContext* context);
	void SetIB(ID3D11DeviceContext* context);
	void SetID(std::string id) { this->id = id; }

public:
	//������
	Mesh(const std::string& id) : mVB(0), mIB(0), InstanceBuffer(0), vertexBufferCount(0), id(id) {}
	Mesh() : mVB(0), mIB(0), InstanceBuffer(0), vertexBufferCount(0), id("temp") {}
	~Mesh() {
		ID3D11Buffer* VB = mVB;
		ID3D11Buffer* IB = mIB;
		ID3D11Buffer* inst = InstanceBuffer;
		ReleaseCOM(mVB); ReleaseCOM(mIB); ReleaseCOM(InstanceBuffer); 
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
	
	void InitBuffers(ID3D11Device* device);

public:
	void Draw(ID3D11DeviceContext* context, UINT subsetIdx);

	ID3D11Buffer* GetVB() { return mVB; }
	ID3D11Buffer* GetIB() { return mVB; }
	ID3D11Buffer* GetInstanceBuffer() { return InstanceBuffer; }

	int GetSubsetLength() { return subsets.size(); }
	const std::vector<Subset>& GetSubsets(){ return subsets; }
	const std::vector<MyVertex::BasicVertex>& GetVertices() { return vertices; }
	const std::vector<UINT>& GetIndices() { return indices; }

public:
	bool operator==(const Mesh& mesh) { return id < mesh.id; }
	bool operator==(const std::string& name) { return id == name; }
};

