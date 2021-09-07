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

class ObjectLoader
{
private:
	const aiScene* m_pScene;
	//�޽������� ������ �����̳ʵ�
	vector<MyVertex::BasicVertex>* vertices;
	vector<UINT>* indices; 
	vector<Subset>* subsets;
	vector<GeneralMaterial>* materials;

	//Mesh���� vector�� �ʱ�ȭ
	void InitMeshData(vector<MyVertex::BasicVertex>* vertices,
		vector<UINT>* indices,
		vector<Subset>* subsets);
	
	//Material vector �ʱ�ȭ
	void InitMaterialData(vector<GeneralMaterial>* materials);
	void SetMaterial(const int& matNumOfMesh);

	//���Ž��
	void NodeTravel(aiNode* node);
	//�Ž������Լ�
	void SetMesh(aiMesh* mesh);
public:
	int vertexCount;
	int indexCount;
	//������
	ObjectLoader() : m_pScene(0), vertices(0), indices(0), subsets(0), materials(0) {}
	
	//�������� �о�� scene �ʱ�ȭ
	void InitScene(const string & fileName);
	//���� �����̳� �ѹ��� �ʱ�ȭ
	void InitAll(vector<MyVertex::BasicVertex>* vertices,
		vector<UINT>* indices,
		vector<Subset>* subsets,
		vector<GeneralMaterial>* materials);

	//Data �б�
	bool LoadData();
	
};