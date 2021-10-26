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



#pragma comment(lib, "assimp.lib")

using namespace std;

class AssimpLoader
{
private:

	std::string currentFileName;
private:
	const aiScene* m_pScene;
	//�޽������� ������ �����̳ʵ�
	vector<MyVertex::BasicVertex> vertices;
	vector<UINT> indices; 
	vector<Subset> subsets;
	vector<GeneralMaterial> materials;
		
private:
	//Material vector �ʱ�ȭ
	void SetMaterial(int matNumOfMesh);
	//���� �����̳� �ѹ��� �ʱ�ȭ
	void InitContainer();

	//���Ž��
	void NodeTravel(aiNode* node);
	//�Ž������Լ�
	void SetMesh(aiMesh* mesh);
public:
	int vertexCount;
	int indexCount;
public:
	//������
	AssimpLoader() : m_pScene(0), vertices(0), indices(0), subsets(0), materials(0)
	,vertexCount(0), indexCount(0){}
	//�Ҹ���
	~AssimpLoader() { aiReleaseImport(m_pScene); }
	
public:
	//�������� �о�� scene �ʱ�ȭ
	void InitScene(const string & fileName);
	//Data �б�
	bool LoadData();

public:
	vector<MyVertex::BasicVertex> GetVertices() { return vertices; }
	vector<UINT> GetIndices() { return indices; }
	vector<Subset> GetSubsets() { return subsets; }
	vector<GeneralMaterial> GetMaterials(){ return materials; }
	bool HasBone() { return false; } //�ӽ÷� false�� ��ȯ
	bool IsEmpty() { return m_pScene == nullptr; }
	bool EqualFileName(const std::string& fileName) { return currentFileName == fileName; }
};