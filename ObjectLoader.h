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
	
	void InitScene();
	
	
public:
	int vertexCount;
	int indexCount;
	//생성자에서 scene 초기화
	ObjectLoader(const string& fileName);
	//Mesh관련 vector들 초기화
	void InitMeshData(vector<MyVertex::BasicVertex>* vertices,
		vector<UINT>* indices,
		vector<Subset>* subsets);
	//Material vector 초기화
	void InitMaterialData(vector<GeneralMaterial>* materials);
	void SetMaterial(const int& matNumOfMesh);

	void NodeTravel(aiNode* node);
	void SetMesh(aiMesh* mesh);
	
	
	//void InitMesh(UINT index, const aiMesh* pMesh);
			
	//void printVerticesSize()
	//{
	//	WCHAR strBuffer[100];
	//	swprintf(strBuffer, L"\nIn Obj Class Vertex Vector Size : %d\n My Obj Class face Vector Size : %d\n"
	//		, vertices.size(), indices.size() / 3);
	//	OutputDebugStringW(strBuffer);

	//	swprintf(strBuffer, L"vertices.capacity : %d , vertices.size : %d"
	//		, vertices.capacity(), vertices.size());
	//	OutputDebugStringW(strBuffer);
	//}
};