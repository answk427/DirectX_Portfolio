#pragma once
#include "Mesh.h"
#include "AssimpLoader.h"
#include "ComponentMgr.h"
#include "MeshReaderWriter.h"
#include <GeometryGenerator.h>

#define MESHES_SIZE 10000

class MeshMgr
{
private:
	ID3D11Device* device;	
	vector<Mesh> meshes;
	MeshReaderWriter& meshReaderWriter;
	unique_ptr<GeometryGenerator> geo;
	

public:
	MeshMgr(ID3D11Device* device) : device(device), meshReaderWriter(MeshReaderWriter::Instance()),
		geo(make_unique<GeometryGenerator>())
	{ meshes.reserve(MESHES_SIZE); }
	MeshMgr() : meshReaderWriter(MeshReaderWriter::Instance()){}
	~MeshMgr();
public:
	//assimpLoader 사용
	Mesh* CreateMeshFromFile(const std::string fileName, AssimpMesh& loader);
	//binaryFile 읽기
	Mesh* CreateMeshFromFile(const std::wstring fileName);
	//기본 상자 생성
	Mesh* CreateBasicBox(float width, float height, float depth);
	
	


	void Init(ID3D11Device* device) { this->device = device; }

	Mesh* GetMesh(const std::string meshId);
	
};