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
	//�޽������� ������ �����̳ʵ�
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
	bool HasBone() { return false; } //�ӽ÷� false�� ��ȯ
};

class NodeStruct
{
private:
	wstring name;
	XMFLOAT4X4 mat;

public:
	//Assimp���� �о���� �޽�
	AssimpMesh* assimpMesh;
	//���� ����� ��������
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
	//���� �����̳� �ѹ��� �ʱ�ȭ
	//void InitContainer();

	//���Ž��
	void AssimpLoader::NodeTravel();
	void AssimpLoader::NodeTravel(NodeStruct& parent, aiNode * node);
	//�Ž������Լ�
	void AssimpLoader::SetMesh(AssimpMesh& assimpMesh, aiMesh * mesh);
	//Material ������ �����Լ�
	GeneralMaterial SetMaterial(int matNumOfMesh);
	
	//aiMatrix->xmfloat4x4 ��ȯ
	inline XMFLOAT4X4 ConvertMatrix(aiMatrix4x4& mat);

public:
	//������
	AssimpLoader() : m_pScene(0), root(0){}
	//�Ҹ���
	~AssimpLoader()
	{
		aiReleaseImport(m_pScene);
		if (!root)
			delete root;
	}
	
public:
	//�������� �о�� scene �ʱ�ȭ
	void InitScene(const string & fileName);
	//Data �б�
	bool LoadData();
	bool IsEmpty() { return m_pScene == nullptr; }
	bool EqualFileName(const std::string& fileName) { return currentFileName == fileName; }
	NodeStruct* getRoot() { return root; }
};