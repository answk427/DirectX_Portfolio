#include "MeshMgr.h"

bool sortComp(Mesh& MeshA, Mesh& MeshB)
{
	return MeshA.id < MeshB.id;
}

Mesh* MeshMgr::CreateMeshFromFile(const std::string fileName, AssimpMesh& loader)
{
	
	//이미 있는 매쉬면 반환
	auto it = std::find(meshes.begin(), meshes.end(), fileName);
	if (it != meshes.end())
	{
		return &(*it);
	}
	
			
	meshes.push_back(Mesh(fileName));
	meshes.back().Init(device, loader.GetVertices(), loader.GetIndices(), loader.GetSubsets());
		
	return &meshes[meshes.size()-1];
}


