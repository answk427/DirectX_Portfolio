#include "MeshMgr.h"

bool sortComp(Mesh& MeshA, Mesh& MeshB)
{
	return MeshA.id < MeshB.id;
}

Mesh* MeshMgr::GetMesh(const std::string meshId)
{
	//�̹� �ִ� �Ž��� ��ȯ
	auto it = std::find(meshes.begin(), meshes.end(), meshId);
	if (it != meshes.end())
	{
		return &(*it);
	}
	return nullptr;
}




MeshMgr::~MeshMgr()
{
	for (auto& mesh : meshes)
	{
		meshReaderWriter.WriteFile(&mesh);
	}
}

Mesh* MeshMgr::CreateMeshFromFile(const std::string fileName, AssimpMesh& loader)
{	
	//�̹� �ִ� �Ž��� ��ȯ
	auto it = std::find(meshes.begin(), meshes.end(), fileName);
	if (it != meshes.end())
	{
		return &(*it);
	}
				
	meshes.push_back(Mesh(fileName));
	meshes.back().Init(device, loader.GetVertices(), loader.GetIndices(), loader.GetSubsets());
	AABB_MaxMin* aabb = loader.GetAABB_MaxMin();
	meshes.back().SetAABB_MaxMin(aabb->m_max, aabb->m_min);
	
		
	return &meshes[meshes.size()-1];
}

Mesh * MeshMgr::CreateMeshFromFile(const std::wstring fileName)
{
	Mesh mesh{};
	
	//�б� �����ϸ� null ������ ��ȯ
	if (!meshReaderWriter.ReadFile(fileName, (char*)(&mesh)))
		return nullptr;
	
	meshes.push_back(mesh);
	meshes.back().InitBuffers(device);
	
	
	return &meshes[meshes.size()-1];
}


