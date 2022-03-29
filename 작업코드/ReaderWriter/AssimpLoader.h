#pragma once

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <string>

#include "Mesh.h"
#include "BufferResource.h"
#include "Renderer.h"



class AssimpMesh
{
private:
	friend class AssimpLoader;
	//메쉬구조를 저장할 컨테이너들
	vector<MyVertex::BasicVertex> vertices;
	vector<UINT> indices;
	vector<Subset> subsets;
	vector<GeneralMaterial> materials;
	AABB_MaxMin* m_AABB_MaxMin;
public:
	
	int vertexCount;
	int indexCount;
public:
	AssimpMesh() :vertexCount(0), indexCount(0), m_AABB_MaxMin(0) {}
	~AssimpMesh()
	{
		if (m_AABB_MaxMin != nullptr)
		{
			delete m_AABB_MaxMin;
			m_AABB_MaxMin = nullptr;
		}
	}
public:
	vector<MyVertex::BasicVertex> GetVertices() { return vertices; }
	vector<UINT> GetIndices() { return indices; }
	vector<Subset> GetSubsets() { return subsets; }
	vector<GeneralMaterial> GetMaterials() { return materials; }
	AABB_MaxMin* GetAABB_MaxMin() { return m_AABB_MaxMin; }
	bool HasBone() { return false; } //임시로 false만 반환
};

class NodeStruct
{
private:
	wstring name;
	XMFLOAT4X4 mat;

public:
	//Assimp에서 읽어들인 메쉬
	AssimpMesh* assimpMesh;
	//현재 노드의 하위노드들
	vector<NodeStruct> childs;
public:
	NodeStruct(const wstring& nodeName, const XMFLOAT4X4 nodeMat) : name(nodeName), mat(nodeMat), assimpMesh(0){}
	~NodeStruct()
	{
		if(!assimpMesh)
			delete assimpMesh;
	}
public:
	wstring GetName() { return name; }
	XMFLOAT4X4 GetMatrix() { return mat; }
};



#pragma comment(lib, "assimp.lib")

using namespace std;

class AssimpLoader
{
private:
	std::string currentFileName;
	NodeStruct* root;
private:
	const aiScene* m_pScene;
			
private:
	//적재 컨테이너 한번에 초기화
	//void InitContainer();

	//노드탐색
	void AssimpLoader::NodeTravel();
	void AssimpLoader::NodeTravel(NodeStruct& parent, aiNode * node);
	//매쉬적재함수
	void AssimpLoader::SetMesh(AssimpMesh& assimpMesh, aiMesh * mesh);
	//Material 데이터 적재함수
	GeneralMaterial SetMaterial(int matNumOfMesh);
	
	//aiMatrix->xmfloat4x4 변환
	inline XMFLOAT4X4 ConvertMatrix(aiMatrix4x4& mat);

public:
	//생성자
	AssimpLoader() : m_pScene(0), root(0){}
	//소멸자
	~AssimpLoader()
	{
		aiReleaseImport(m_pScene);
		if (!root)
			delete root;
	}
	
public:
	//모델파일을 읽어와 scene 초기화
	void InitScene(const string & fileName);
	//Data 읽기
	bool LoadData();
	bool IsEmpty() { return m_pScene == nullptr; }
	bool EqualFileName(const std::string& fileName) { return currentFileName == fileName; }
	NodeStruct* getRoot() { return root; }
};