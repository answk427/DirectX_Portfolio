#pragma once
#include "Mesh.h"
#include "AssimpLoader.h"
#include "ComponentMgr.h"

#define MESHES_SIZE 10000

class MeshMgr
{
private:
	ID3D11Device* device;
	
	vector<Mesh> meshes;


public:
	MeshMgr(ID3D11Device* device) : device(device) { meshes.reserve(MESHES_SIZE); }
	MeshMgr() {}
	~MeshMgr() {}
public:
	Mesh* CreateMeshFromFile(const std::string fileName, AssimpMesh& loader);
	void Init(ID3D11Device* device) { this->device = device; }
	
	
};