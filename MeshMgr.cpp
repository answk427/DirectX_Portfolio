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

Mesh * MeshMgr::CreateBasicBox(float width, float height, float depth)
{
	//�̹� �ִ� �Ž��� ��ȯ
	auto it = std::find(meshes.begin(), meshes.end(), "BasicBox");
	if (it != meshes.end())
	{
		return &(*it);
	}

	meshes.push_back(Mesh("BasicBox"));
	//Box ����,�ε��� �ʱ�ȭ
	geo->CreateBox(width, height, depth, meshes.back());
	//��������, �ε������� �ʱ�ȭ
	meshes.back().InitBuffers(device);
	
	//aabb �ʱ�ȭ
	meshes.back().SetAABB_MaxMin(XMFLOAT3(width *0.5, height *0.5, depth *0.5), XMFLOAT3(-width*0.5, -height*0.5, -depth*0.5));
	
	return &meshes[meshes.size() - 1];
}

Mesh * MeshMgr::CreateBasicGrid(float width, float depth, UINT m, UINT n)
{
	static int num = 0;
	meshes.push_back(Mesh("BasicGrid" + std::to_string(num++)));
	
	//Box ����,�ε��� �ʱ�ȭ
	geo->CreateGrid(width, depth, m, n, meshes.back());
	//��������, �ε������� �ʱ�ȭ
	meshes.back().InitBuffers(device);

	//aabb �ʱ�ȭ
	meshes.back().SetAABB_MaxMin(XMFLOAT3(width *0.5, 0.0f, depth *0.5), XMFLOAT3(-width*0.5, 0.0f, -depth*0.5));

	return &meshes[meshes.size() - 1];
}


