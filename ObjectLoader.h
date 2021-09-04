#pragma once

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <string>

#include "Mesh.h"




#pragma comment(lib, "assimp.lib")

using namespace std;

class ObjectLoader
{
private:
	const aiScene* m_pScene;
	
	void InitScene();
	
public:
	ObjectLoader(const string& fileName);
	
	
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