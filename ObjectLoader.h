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
	//메쉬구조를 저장할 컨테이너들
	vector<MyVertex::BasicVertex>* vertices;
	vector<UINT>* indices; 
	vector<Subset>* subsets;
	vector<GeneralMaterial>* materials;

	//Mesh관련 vector들 초기화
	void InitMeshData(vector<MyVertex::BasicVertex>* vertices,
		vector<UINT>* indices,
		vector<Subset>* subsets);
	
	//Material vector 초기화
	void InitMaterialData(vector<GeneralMaterial>* materials);
	void SetMaterial(const int& matNumOfMesh);

	//노드탐색
	void NodeTravel(aiNode* node);
	//매쉬적재함수
	void SetMesh(aiMesh* mesh);
public:
	int vertexCount;
	int indexCount;
	//생성자
	ObjectLoader() : m_pScene(0), vertices(0), indices(0), subsets(0), materials(0) {}
	
	//모델파일을 읽어와 scene 초기화
	void InitScene(const string & fileName);
	//적재 컨테이너 한번에 초기화
	void InitAll(vector<MyVertex::BasicVertex>* vertices,
		vector<UINT>* indices,
		vector<Subset>* subsets,
		vector<GeneralMaterial>* materials);

	//Data 읽기
	bool LoadData();
	
};