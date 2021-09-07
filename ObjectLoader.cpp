#include "ObjectLoader.h"
#include <atlconv.h>


void ObjectLoader::InitScene(const string& fileName)
{
	m_pScene = aiImportFile(fileName.c_str(),
		aiProcess_GenBoundingBoxes | //BoundingBox ���?

		aiProcess_CalcTangentSpace | //ź��Ʈ���� ��ǥ�� ��� tangent, bitangent

		aiProcess_ConvertToLeftHanded | //�޼���ǥ��� ��ȯ

		aiProcess_Triangulate | //�Ž��� �����ϴ� �ٰ��� ���� �ﰢ������ ����

		aiProcess_GenSmoothNormals | //�ε巯�� normal���� ����

		aiProcess_SplitLargeMeshes | //�Ž��� ����

		aiProcess_LimitBoneWeights | //������ ������ �ִ� ���� ���� 4���� ����

		aiProcess_ValidateDataStructure | //������ ������ ��ȿ���� �˻�

		aiProcess_ImproveCacheLocality | //ĳ�� ����ȭ

		aiProcess_RemoveRedundantMaterials | //�ߺ� ���׸��� ����

		aiProcess_FixInfacingNormals | //������ ���ϴ� normal���� ����

		aiProcess_PopulateArmatureData | //������� �÷��� �ϴ� �������� ���������� Ȱ��ȭ

		aiProcess_SortByPType | //����Ÿ���� ������Ƽ��� ������ '������' �Ž��� ����

		aiProcess_FindInstances  //�ߺ� �޽� ����
	);

	//m_pScene = aiImportFile(fileName.c_str(), aiProcess_JoinIdenticalVertices |        // ������ ������ ����, �ε��� ����ȭ

	//	aiProcess_ValidateDataStructure |        // �δ��� ����� ����

	//	aiProcess_ImproveCacheLocality |        // ��� ������ ĳ����ġ�� ����

	//	aiProcess_RemoveRedundantMaterials |    // �ߺ��� ���͸��� ����

	//	aiProcess_GenUVCoords |                    // ����, ������, ���� �� ��� ������ ������ UV�� ��ȯ

	//	aiProcess_TransformUVCoords |            // UV ��ȯ ó���� (�����ϸ�, ��ȯ...)

	//	aiProcess_FindInstances |                // �ν��Ͻ��� �Ž��� �˻��Ͽ� �ϳ��� �����Ϳ� ���� ������ ����

	//	aiProcess_LimitBoneWeights |            // ������ ���� ����ġ�� �ִ� 4���� ����

	//	aiProcess_OptimizeMeshes |                // ������ ��� ���� �Ž��� ����

	//	aiProcess_GenSmoothNormals |            // �ε巯�� �븻����(��������) ����

	//	aiProcess_SplitLargeMeshes |            // �Ŵ��� �ϳ��� �Ž��� �����Ž���� ��Ȱ(����)

	//	aiProcess_Triangulate |                    // 3�� �̻��� �𼭸��� ���� �ٰ��� ���� �ﰢ������ ����(����)

	//	aiProcess_ConvertToLeftHanded |            // D3D�� �޼���ǥ��� ��ȯ

	//	aiProcess_SortByPType);                    // ����Ÿ���� ������Ƽ��� ������ '������' �Ž��� ����

	if (m_pScene) //����� ������ ���������� �ε����� ��
	{
		
	}
	else
	{
		HR(-1);
	}
}

bool ObjectLoader::LoadData()
{
	//Init�� ����� �ȵ� ��Ȳ
	if (m_pScene == NULL || vertices == NULL || indices == NULL || subsets == NULL || materials == NULL)
		return false;
	
	NodeTravel(m_pScene->mRootNode);
}

void ObjectLoader::InitAll(vector<MyVertex::BasicVertex>* vertices, vector<UINT>* indices, vector<Subset>* subsets, vector<GeneralMaterial>* materials)
{
	InitMeshData(vertices, indices, subsets);
	InitMaterialData(materials);
}



void ObjectLoader::InitMeshData(vector<MyVertex::BasicVertex>* vertices,
	vector<UINT>* indices,
	vector<Subset>* subsets)
{
	this->vertices = vertices;
	this->indices = indices;
	this->subsets = subsets;
}

void ObjectLoader::InitMaterialData(vector<GeneralMaterial>* materials)
{
	this->materials = materials;
	//material�� ������ŭ �ʱ�ȭ
	materials->assign(m_pScene->mNumMaterials, GeneralMaterial());
}

void ObjectLoader::SetMaterial(const int & matNumOfMesh)
{
	aiMaterial* aiMat = m_pScene->mMaterials[matNumOfMesh];
		
	aiColor4D color;
	
	//Material�� Diffuse �� ����
	aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	(*materials)[matNumOfMesh].basicMat.Diffuse = { color.r, color.g, color.b, 1.0f };
	
	//Material�� ambient �� ����
	aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	(*materials)[matNumOfMesh].basicMat.Ambient = { color.r, color.g, color.b, 1.0f };
	
	//Material�� Specular �� ����
	aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	(*materials)[matNumOfMesh].basicMat.Specular = { color.r, color.g, color.b, color.a };

	//*************Texture ���� **************//
	aiString* fileName;
	
	//A2W(��Ƽ�ڵ� -> ��Ƽ����Ʈ ��ȯ �Լ�)�� ����ϱ����� ��ũ��
	USES_CONVERSION; 

	//DiffuseTexture ��� ����
	aiMat->GetTexture(aiTextureType_DIFFUSE, 0, fileName);
	(*materials)[matNumOfMesh].diffuseMapName = A2W(fileName->C_Str());
	
	
	//SpecularTexture ��� ����
	aiMat->GetTexture(aiTextureType_SPECULAR, 0, fileName);
	(*materials)[matNumOfMesh].specularMap= A2W(fileName->C_Str());

	//NormalTexture ��� ����
	aiMat->GetTexture(aiTextureType_NORMALS, 0, fileName);
	(*materials)[matNumOfMesh].normalMapName = A2W(fileName->C_Str());
}



void ObjectLoader::NodeTravel(aiNode * node)
{
	//�ش� ����� �Ž��� ����
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		SetMesh(mesh);
	}
	
	//��� �ڽĳ�带 ��ȸ
	for (int i = 0; i < node->mNumChildren; i++)
		NodeTravel(node->mChildren[i]);
	
	return;
}

void ObjectLoader::SetMesh(aiMesh * mesh)
{
	
	Subset tempSubset;
	//material�ε��� 
	tempSubset.materialNum = mesh->mMaterialIndex;
	//���� vertex, index ������ �� ���� subset�� ������
	tempSubset.VertexStart = vertexCount;
	tempSubset.IndexStart = indexCount;
	
	//�� ������ ������ ����
	vertexCount += mesh->mNumVertices;
	vertices->reserve(vertexCount+10);

	//index�� �� ����
	indexCount += mesh->mNumFaces*3;
	indices->reserve(indexCount + 10);

	tempSubset.VertexCount = mesh->mNumVertices;
	tempSubset.IndexCount = mesh->mNumFaces*3;
	
	subsets->push_back(tempSubset);
	
	SetMaterial(mesh->mMaterialIndex);

	//���� ����ü ������
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		MyVertex::BasicVertex tempVertex;

		//position ��ǥ ����
		if (mesh->HasPositions())
		{
			aiVector3D& position = mesh->mVertices[i];
			tempVertex.pos = XMFLOAT3(position.x, position.y, position.z);
		}
		
		//Texture ��ǥ ����
		if (mesh->HasTextureCoords(0))
		{
			//�迭�� �ּҷ� �Ű����� �ѱ�
			tempVertex.tex = XMFLOAT2(&mesh->mTextureCoords[0][i].x);
		}

		//�������� ����
		if (mesh->HasNormals())
		{
			aiVector3D& normal = mesh->mNormals[i];
			tempVertex.normal = XMFLOAT3(normal.x, normal.y, normal.z);
		}

		//�������� ����
		if (mesh->HasTangentsAndBitangents())
		{
			aiVector3D& tangent = mesh->mTangents[i];
			aiVector3D& biTangent = mesh->mBitangents[i];
			
			tempVertex.tan = XMFLOAT3(tangent.x, tangent.y, tangent.z);
			tempVertex.biTan = XMFLOAT3(biTangent.x, biTangent.y, biTangent.z);
		}
		vertices->push_back(tempVertex);
	}

	
	//index ����
	if (mesh->HasFaces())
	{
		//�ﰢ������ �̷���� ����� �ε��� ����
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			indices->push_back(face.mIndices[0]);
			indices->push_back(face.mIndices[1]);
			indices->push_back(face.mIndices[2]);
		}
	}

	/*if (mesh->HasVertexColors(mesh->GetNumColorChannels()))
	{
		mesh->color
	}*/

	if (mesh->HasBones())
	{

	}
	
}
