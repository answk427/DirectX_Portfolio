#pragma once

#include <D3D11.h>
#include <vector>
#include <d3dUtil.h>
#include "BufferResource.h"
#include <UtilFunctions.h>
#include <unordered_map>

using namespace std;

#define MAX_INSTSANCING 1000

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
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3D11Buffer* m_InstanceBuffer;
	XNA::AxisAlignedBox m_AABB;
	AABB_MaxMin m_Aabb_MaxMin;
	bool m_instancing; //인스턴싱 렌더링 여부를 나타내는 변수


private:
	UINT vertexBufferCount;
	//해당 변수만큼 인스턴스버퍼의 크기를 설정한다.
	UINT m_instanceBufferSize;
public:
	//버퍼초기화 함수들
	//정점버퍼 생성
	void InitVB(ID3D11Device* device);
	void InitIB(ID3D11Device* device);

	//여러 vertex 구조체를 이용해 버퍼를 초기화하는 함수
	template <typename vertex>
	void InitVB(ID3D11Device* device, std::vector<vertex> vertices);
	
	void InitInstanceBuffer(ID3D11Device* device, UINT bufferSize);
	//현재 클래스의 m_instanceBufferSize 멤버변수로 버퍼사이즈 설정
	void InitInstanceBuffer(ID3D11Device* device);

	//여러 vertex 구조체를 이용해 쓰기가능 버퍼를 초기화하는 함수
	template <typename vertex>
	void InitWritableVB(ID3D11Device* device, UINT bufferSize);
	void InitWritableIB(ID3D11Device* device, UINT bufferSize);

private:
	//ObjectLoader에서 매쉬정보들을 전달받는 함수
	void SetVertices(vector<MyVertex::BasicVertex>& vertexSrc);
	void SetIndices(vector<UINT>& indexSrc);
	void SetSubsets(vector<Subset>& subsetSrc);

	
public:
	std::string id;

	//반직선 검출등에 사용할 정점과 색인		
	vector<MyVertex::BasicVertex> vertices;
	vector<UINT> indices;
	vector<Subset> subsets;
	//실제 instancingData를 담고 있는 vector
	std::vector<InstancingData*> InstancingDatas;
	//이번 렌더링에서 사용할 instancingData의 index.
	vector<UINT> enableInstancingIndexes;

#pragma region InstancingTextureBuffer
	//Texture 배열에서 사용할 텍스쳐들의 경로
	vector<std::wstring>* textureNames{0};
	//Subset별로 사용할 Texture배열들
	vector<ID3D11ShaderResourceView*> textureArrays;
	//Texture 배열의 크기가 달라졌거나 다시 만들어야할 경우 사용하는 함수
	void CreateTextureArrayResourceView(ID3D11Device* device, ID3D11DeviceContext* context);
	//Texture 배열의 부분만 수정하는 함수
	void ModifyTextureArraySubResource(ID3D11Device* device, ID3D11DeviceContext* context, 
		UINT rendererIdx, UINT subsetIdx, const std::wstring& textureName);
#pragma endregion

	
	
	//vertex, index버퍼 입력조립기에 바인딩.
	void SetVB(ID3D11DeviceContext* context);
	void SetInstanceVB(ID3D11DeviceContext* context);
	void SetIB(ID3D11DeviceContext* context);
	void SetID(std::string id) { this->id = id; }
	
	//aabb 관련 함수
	void SetAABB_MaxMin(XMFLOAT3& maxV, XMFLOAT3& minV);
	AABB_MaxMin& GetAABB_MaxMin() { return m_Aabb_MaxMin; }
	XNA::AxisAlignedBox& GetAABB() { return m_AABB; }
	const XNA::AxisAlignedBox& GetAABB() const { return m_AABB; }

	//instancing 관련 함수
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
	//생성자
	Mesh(const std::string& id) : mVB(0), mIB(0), m_InstanceBuffer(0), vertexBufferCount(0),
		id(id), m_instanceBufferSize(MAX_INSTSANCING), m_instancing(0)
	{	SetInstanceBufferSize(MAX_INSTSANCING);	}
	Mesh() : mVB(0), mIB(0), m_InstanceBuffer(0), vertexBufferCount(0), id("temp"), m_instanceBufferSize(MAX_INSTSANCING), m_instancing(0)
	{	SetInstanceBufferSize(MAX_INSTSANCING);	}
	~Mesh(); 
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

	//기본 Subset 1개인걸로 생성
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

	//bufferDesc 작성
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(vertex) * vertices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //구조적버퍼에 저장된 원소 하나의 크기, 구조적버퍼 사용할때 필요
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
