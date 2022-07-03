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


struct AssimpSkinnedVertex
{
	AssimpSkinnedVertex()  {	}
	//������ �޴� �� �̸���� ����ġ
	std::vector<std::string> nodeName;
	std::vector<float> weights;
};

struct AssimpBone
{
	XMFLOAT4X4 offsetMat;
};

struct Float3Key //Ű�������� Ű ������,�����Ͽ� ���
{
	Float3Key(double timeVar, float px, float py, float pz) : float3Key(px, py, pz), time(timeVar) {}
	XMFLOAT3 float3Key;
	double time;
};
struct Float4Key
{
	Float4Key(double timeVar, float px, float py, float pz, float pw) : quaternionKey(px, py, pz, pw), time(timeVar) {}
	XMFLOAT4 quaternionKey;
	double time;
};

//�� ��忡���� �ð��� ���� �ִϸ��̼� Ű�����ӵ��� �����ϴ� ����ü
struct AssimpAnimationFrame
{
	AssimpAnimationFrame() {}
	/*AssimpAnimationFrame(double posTime, float px, float py, float pz,
		float quatTime, float rx, float ry, float rz, float rw,
		float scaleTime, float sx, float sy, float sz) 
			:	positionKey(posTime, px, py, pz), quaternionKey(quatTime, rx, ry, rz, rw), scalingKey(scaleTime, sx, sy, sz) {}*/
	
	std::vector<Float3Key> positionKey;
	std::vector<Float4Key> quaternionKey;
	std::vector<Float3Key> scalingKey;
};

struct AssimpAnimation
{
	double duration;
	double ticksPerSecond;
	std::map<std::string, AssimpAnimationFrame> bones;
};


class AssimpMesh
{
private:
	friend class AssimpLoader;
	//�޽������� ������ �����̳ʵ�
	std::vector<MyVertex::BasicVertex> vertices;
	std::vector<UINT> indices;
	std::vector<AssimpSkinnedVertex> skinnedVertices; //��, ����ġ
	std::vector<Subset> subsets;
	std::vector<GeneralMaterial> materials;
	AABB_MaxMin* m_AABB_MaxMin;
public:
	
	int vertexCount;
	int indexCount;
public:
	AssimpMesh() :vertexCount(0), indexCount(0), m_AABB_MaxMin(0) {}
	~AssimpMesh()
	{
		if (m_AABB_MaxMin != nullptr)
		{
			delete m_AABB_MaxMin;
			m_AABB_MaxMin = nullptr;
		}
	}
public:
	std::vector<MyVertex::BasicVertex> GetVertices() { return vertices; }
	std::vector<UINT> GetIndices() { return indices; }
	std::vector<Subset> GetSubsets() { return subsets; }
	std::vector<GeneralMaterial> GetMaterials() { return materials; }
	AABB_MaxMin* GetAABB_MaxMin() { return m_AABB_MaxMin; }
	bool HasBone() { return false; } //�ӽ÷� false�� ��ȯ
};

class NodeStruct
{
private:
	std::wstring name;
	XMFLOAT4X4 mat;
	XMFLOAT4X4 offsetMat;

public:
	//Assimp���� �о���� �޽�
	AssimpMesh* assimpMesh;
	//���� ����� ��������
	std::vector<NodeStruct> childs;
public:
	NodeStruct(const std::wstring& nodeName, const XMFLOAT4X4 nodeMat) : name(nodeName), mat(nodeMat), assimpMesh(0)
	{
		XMStoreFloat4x4(&offsetMat, XMMatrixIdentity()); //�⺻ ������ķ� �ʱ�ȭ
	}
	
	~NodeStruct()
	{	
		if(!assimpMesh) 
			delete assimpMesh;
	}
public:
	std::wstring GetName() { return name; }
	XMFLOAT4X4 GetMatrix() { return mat; }
	XMFLOAT4X4 GetOffsetMatrix() { return offsetMat; }
};



#pragma comment(lib, "assimp.lib")

class AssimpLoader
{
private:
	std::string currentFileName;
	NodeStruct* root;
	std::map<std::string, AssimpBone> m_assimpBones;
	std::map<std::string, AssimpAnimation> m_animations;
private:
	const aiScene* m_pScene;
			
private:
	//���� �����̳� �ѹ��� �ʱ�ȭ
	//void InitContainer();

	//���Ž��
	void AssimpLoader::NodeTravel();
	void AssimpLoader::NodeTravel(NodeStruct& parent, aiNode * node);
	//�Ž������Լ�
	void AssimpLoader::SetMesh(AssimpMesh& assimpMesh, aiMesh * mesh);
	//Material ������ �����Լ�
	GeneralMaterial SetMaterial(int matNumOfMesh);
	//�ִϸ��̼� �ε� �Լ�
	void LoadAnimation(const aiAnimation* animation);
	
	//aiMatrix->xmfloat4x4 ��ȯ
	inline XMFLOAT4X4 ConvertMatrix(aiMatrix4x4& mat);

public:
	//������
	AssimpLoader() : m_pScene(0), root(0){}
	//�Ҹ���
	~AssimpLoader()
	{
		aiReleaseImport(m_pScene);
		if (!root)
			delete root;
	}
	
public:
	//�������� �о�� scene �ʱ�ȭ
	void InitScene(const std::string & fileName);
	//Data �б�
	bool LoadData();
	bool IsEmpty() { return m_pScene == nullptr; }
	bool EqualFileName(const std::string& fileName) { return currentFileName == fileName; }
	NodeStruct* getRoot() { return root; }
	std::map<std::string, AssimpBone>& getAssimpBones() { return m_assimpBones; }
	std::map<std::string, AssimpAnimation>& getAssimpAnimations() { return m_animations; }
};