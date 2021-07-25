#pragma once

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <string>

#include "Vertex.h"
#include "Effects.h"

#pragma comment(lib, "assimp.lib")

using namespace std;

struct mesh
{
	vector<Vertex::Basic32> vertices;
	vector<int> indices;
	UINT materialIndex;
	mesh() {
		materialIndex = 0;
	}
};

class ObjectLoader
{
private:
	const aiScene* m_pScene;
	vector<mesh> m_meshes; //meshÀÇ ÁýÇÕ
	ID3D11Buffer* m_VB;
	ID3D11Buffer* m_IB;

	vector<Vertex::Basic32> vertices;
	vector<UINT32> indices;
	

	int m_numMaterial;
	int m_numBones;
	

	void InitScene();
	void InitMesh(UINT index, const aiMesh* pMesh);
	

public:
	ObjectLoader(const string& fileName);
	void Draw(ID3D11DeviceContext* context);
	void setVB(ID3D11Device* device);
	void setIB(ID3D11Device* device);
	int m_numVertex;
	int m_numIndex;
};