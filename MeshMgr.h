#pragma once
#include "Mesh.h"
#include "AssimpLoader.h"
#include "ComponentMgr.h"

class MeshMgr
{
private:
	ID3D11Device* device;
	
	vector<Mesh> meshes;


public:
	MeshMgr(ID3D11Device* device) : device(device) {}
	MeshMgr() {}
	~MeshMgr() {}
public:
	Mesh* CreateMeshFromFile(const std::string fileName, AssimpMesh& loader);
	void Init(ID3D11Device* device) { this->device = device; }
	
	
};