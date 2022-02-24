#pragma once

#include <D3D11.h>
#include <vector>
#include <d3dUtil.h>
#include "BufferResource.h"


using namespace std;

//정점버퍼, 인덱스버퍼를 1개씩 사용할 경우
//(버퍼 하나에 여러 메쉬의 정점,인덱스를 몰아넣는 것)
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
	//버퍼초기화 함수들
	//정점버퍼 생성
	void InitVB(ID3D11Device* device);
	void InitIB(ID3D11Device* device);
	void InitInstanceBuffer(ID3D11Device* device, int bufferSize);

private:
	//ObjectLoader에서 매쉬정보들을 전달받는 함수
	void SetVertices(vector<MyVertex::BasicVertex>& vertexSrc);
	void SetIndices(vector<UINT>& indexSrc);
	void SetSubsets(vector<Subset>& subsetSrc);

	
public:
	std::string id;

	//반직선 검출등에 사용			
	vector<MyVertex::BasicVertex> vertices;
	vector<InstancingData> InstancingDatas;
	vector<UINT> indices;
	
	void SetVB(ID3D11DeviceContext* context);
	void SetIB(ID3D11DeviceContext* context);
	void SetID(std::string id) { this->id = id; }

public:
	//생성자
	Mesh(const std::string& id) : mVB(0), mIB(0), InstanceBuffer(0), vertexBufferCount(0), id(id) {}
	Mesh() : mVB(0), mIB(0), InstanceBuffer(0), vertexBufferCount(0), id("temp") {}
	~Mesh() {
		ID3D11Buffer* VB = mVB;
		ID3D11Buffer* IB = mIB;
		ID3D11Buffer* inst = InstanceBuffer;
		ReleaseCOM(mVB); ReleaseCOM(mIB); ReleaseCOM(InstanceBuffer); 
	}
	//복사생성자
	Mesh(const Mesh& other);
	//이동생성자
	Mesh(Mesh&& other);
	//대입연산자
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

