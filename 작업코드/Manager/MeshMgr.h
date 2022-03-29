#pragma once
#include "Mesh.h"
#include "AssimpLoader.h"
#include "ComponentMgr.h"
#include "MeshReaderWriter.h"


#define MESHES_SIZE 10000

class MeshMgr
{
private:
	ID3D11Device* device;	
	vector<Mesh> meshes;
	MeshReaderWriter& meshReaderWriter;


public:
	MeshMgr(ID3D11Device* device) : device(device), meshReaderWriter(MeshReaderWriter::Instance()) { meshes.reserve(MESHES_SIZE); }
	MeshMgr() : meshReaderWriter(MeshReaderWriter::Instance()){}
	~MeshMgr();
public:
	//assimpLoader 사용
	Mesh* CreateMeshFromFile(const std::string fileName, AssimpMesh& loader);
	//binaryFile 읽기
	Mesh* CreateMeshFromFile(const std::wstring fileName);
	


	void Init(ID3D11Device* device) { this->device = device; }

	Mesh* GetMesh(const std::string meshId);
	
};