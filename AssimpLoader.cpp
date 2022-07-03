#include "AssimpLoader.h"
#include <atlconv.h>


void AssimpLoader::InitScene(const std::string& fileName)
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

	assert(m_pScene);

	if (m_pScene) //����� ������ ���������� �ε����� ��
	{
		currentFileName = fileName;
		m_assimpBones.clear();
		m_animations.clear();
		if (root != nullptr)
		{
			delete root;
			root = nullptr;
		}
	}
	
}

//void AssimpLoader::InitContainer()
//{
//	//���� ���� �۾�		
//	vector<MyVertex::BasicVertex>().swap(vertices);
//	vector<UINT>().swap(indices);
//	vector<Subset>().swap(subsets);
//	vector<GeneralMaterial>().swap(materials);
//}

bool AssimpLoader::LoadData()
{
	if (m_pScene == NULL)
		return false;

	NodeTravel();
	
	if (m_pScene->HasAnimations())
	{
		for (int i = 0; i < m_pScene->mNumAnimations; ++i)
		{
			LoadAnimation(m_pScene->mAnimations[i]);
		}
	}
	
	return true;
}


GeneralMaterial AssimpLoader::SetMaterial(int matNumOfMesh)
{

	aiMaterial* aiMat = m_pScene->mMaterials[matNumOfMesh];
	GeneralMaterial tempMaterial;

	//A2W(�����ڵ� -> ��Ƽ����Ʈ ��ȯ �Լ�)�� ����ϱ����� ��ũ��
	USES_CONVERSION;
	//Material name
	aiString& aiName = aiMat->GetName();
	tempMaterial.name = std::wstring(A2W(aiName.C_Str()));
	
	aiColor4D color;

	//Material�� Diffuse �� ����
	aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	tempMaterial.basicMat.Diffuse = { color.r, color.g, color.b, 1.0f };
	color = { 0,0,0,0 };

	//Material�� ambient �� ����
	aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	tempMaterial.basicMat.Ambient = { color.r, color.g, color.b, 1.0f };
	color = { 0,0,0,0 };

	//Material�� Specular �� ����
	aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	tempMaterial.basicMat.Specular = { color.r, color.g, color.b, color.a };
	color = { 0,0,0,0 };

	//Material�� Reflect �� ����
	aiMat->Get(AI_MATKEY_COLOR_REFLECTIVE, color);
	tempMaterial.basicMat.Reflect = { color.r, color.g, color.b, color.a };

	//*************Texture ���� **************//
	aiString fileName;

	
	
	//DiffuseTexture ��� ����
	aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &fileName);
	tempMaterial.diffuseMapName = A2W(fileName.C_Str());
	fileName.Clear();


	//SpecularTexture ��� ����
	aiMat->GetTexture(aiTextureType_SPECULAR, 0, &fileName);
	tempMaterial.specularMap = A2W(fileName.C_Str());
	fileName.Clear();

	//NormalTexture ��� ����
	aiMat->GetTexture(aiTextureType_NORMALS, 0, &fileName);
	tempMaterial.normalMapName = A2W(fileName.C_Str());

	return tempMaterial;
}

void AssimpLoader::LoadAnimation(const aiAnimation * animation)
{
	m_animations[animation->mName.C_Str()] = AssimpAnimation();
	
	AssimpAnimation& anim = m_animations[animation->mName.C_Str()];

	anim.ticksPerSecond = animation->mTicksPerSecond;
	anim.duration = animation->mDuration;
	
	//channelName = boneName = nodeName
	std::string channelName;
	AssimpAnimationFrame* currBone;
	aiNodeAnim* nodeAnim;
	
	aiVectorKey* currKeyPos;
	aiQuatKey* currKeyRot;
	aiVectorKey* currKeyScale;
	for (int i = 0; i < animation->mNumChannels; ++i)
	{
		nodeAnim= animation->mChannels[i];
		channelName = nodeAnim->mNodeName.C_Str();
		
		if (anim.bones.find(channelName) == anim.bones.end())
		{
			const auto&[it, success] = anim.bones.insert({ channelName, AssimpAnimationFrame() });
			currBone = &(it->second);
		}
		else
			currBone = &anim.bones[channelName];
				
		for (int j = 0; j < nodeAnim->mNumPositionKeys; ++j)
		{
			currKeyPos = &nodeAnim->mPositionKeys[j];
			currKeyRot = &nodeAnim->mRotationKeys[j];
			currKeyScale = &nodeAnim->mScalingKeys[j];
						
			currBone->positionKey.push_back({ currKeyPos->mTime, currKeyPos->mValue.x, currKeyPos->mValue.y, currKeyPos->mValue.z });
			currBone->quaternionKey.push_back({ currKeyRot->mTime, currKeyRot->mValue.x, currKeyRot->mValue.y, currKeyRot->mValue.z, currKeyRot->mValue.w });
			currBone->scalingKey.push_back({ currKeyScale->mTime, currKeyScale->mValue.x, currKeyScale->mValue.y, currKeyScale->mValue.z });
		}		
	}
	
	
}


void AssimpLoader::NodeTravel()
{
	USES_CONVERSION;
	aiNode* node = m_pScene->mRootNode;
	
	//��ȯ��� ����
	root = new NodeStruct(std::wstring(A2W(node->mName.C_Str()))
		, ConvertMatrix(node->mTransformation));
	
	root->GetMatrix();
	//�ش� ����� �Ž��� ����
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		if (root->assimpMesh == nullptr)
			root->assimpMesh = new AssimpMesh();
		SetMesh(*(root->assimpMesh),mesh);
	}
	
	//��� �ڽĳ�带 ��ȸ
	for (int i = 0; i < node->mNumChildren; i++)
		NodeTravel(*root, node->mChildren[i]);
	
}

void AssimpLoader::NodeTravel(NodeStruct& parent ,aiNode * node)
{
	USES_CONVERSION;
	
	
	parent.childs.push_back(NodeStruct(std::wstring(A2W(node->mName.C_Str())),
		ConvertMatrix(node->mTransformation)));
	NodeStruct& current = parent.childs.back();
	
	//�ش� ����� �Ž��� ����
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		if (current.assimpMesh == nullptr)
			current.assimpMesh = new AssimpMesh();
		SetMesh(*(current.assimpMesh),mesh);
	}

	//��� �ڽĳ�带 ��ȸ
	for (int i = 0; i < node->mNumChildren; i++)
		NodeTravel(current,node->mChildren[i]);

	return;
}

void AssimpLoader::SetMesh(AssimpMesh& assimpMesh, aiMesh * mesh)
{
	
	Subset tempSubset;
	//material�ε��� 
	tempSubset.materialNum = mesh->mMaterialIndex;

	//���� vertex, index ������ �� ���� subset�� ������
	tempSubset.VertexStart = assimpMesh.vertexCount;
	tempSubset.IndexStart = assimpMesh.indexCount;

	//�� ������ ������ ����
	assimpMesh.vertexCount += mesh->mNumVertices;
	assimpMesh.vertices.reserve(assimpMesh.vertexCount);
	assimpMesh.skinnedVertices.resize(assimpMesh.vertexCount);

	//index�� �� ����
	assimpMesh.indexCount += mesh->mNumFaces * 3;
	assimpMesh.indices.reserve(assimpMesh.indexCount);

	tempSubset.VertexCount = mesh->mNumVertices;
	tempSubset.IndexCount = mesh->mNumFaces * 3;

	assimpMesh.subsets.push_back(tempSubset);

	//Material ������ ����
	assimpMesh.materials.push_back(SetMaterial(mesh->mMaterialIndex));
	

	
	//AABB BoundingBox ����
	XMFLOAT3 tempAabbMax(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);
	XMFLOAT3 tempAabbMin(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);

	if (mesh->HasBones())
	{
		aiBone* currBone;
		AssimpSkinnedVertex* currSkinnedVertex;
		for (int i = 0; i < mesh->mNumBones; ++i)
		{			
			currBone = mesh->mBones[i];
			if (currBone->mWeights->mVertexId >= assimpMesh.skinnedVertices.size() ||
				currBone->mWeights->mWeight <= 0)
				continue;
			currSkinnedVertex = &assimpMesh.skinnedVertices[currBone->mWeights->mVertexId];
			
			//vertexId��° ����ġ ���� �߰�
			currSkinnedVertex->weights.push_back(currBone->mWeights->mWeight);
			
			
			//����ġ�� ������ ��(���)�� �̸� �߰�
			std::string nodeName = currBone->mName.C_Str();
			currSkinnedVertex->nodeName.push_back(nodeName);
				
			//bone ������ offset��ȯ��� �߰�
			m_assimpBones[nodeName].offsetMat = ConvertMatrix(currBone->mOffsetMatrix);
		}
	}

	if (assimpMesh.m_AABB_MaxMin == nullptr)
		assimpMesh.m_AABB_MaxMin = new AABB_MaxMin(tempAabbMax, tempAabbMin);
	else
	{
		//���� �ٿ���ڽ��� ��
		XMVECTOR aabbMax = XMLoadFloat3(&assimpMesh.m_AABB_MaxMin->m_max);
		XMVECTOR aabbMin = XMLoadFloat3(&assimpMesh.m_AABB_MaxMin->m_min);

		//MaxVector
		XMStoreFloat3(&assimpMesh.m_AABB_MaxMin->m_max, 
			XMVectorMax(aabbMax, XMLoadFloat3(&tempAabbMax)));

		//MinVector
		XMStoreFloat3(&assimpMesh.m_AABB_MaxMin->m_min,
			XMVectorMin(aabbMin, XMLoadFloat3(&tempAabbMin)));
	}
	


	assimpMesh.m_AABB_MaxMin->m_min = { mesh->mAABB.mMin.x,mesh->mAABB.mMin.y,mesh->mAABB.mMin.z };

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
		assimpMesh.vertices.push_back(tempVertex);
	}


	//index ����
	if (mesh->HasFaces())
	{
		//�ﰢ������ �̷���� ����� �ε��� ����
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			assimpMesh.indices.push_back(face.mIndices[0]);
			assimpMesh.indices.push_back(face.mIndices[1]);
			assimpMesh.indices.push_back(face.mIndices[2]);
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

inline XMFLOAT4X4 AssimpLoader::ConvertMatrix(aiMatrix4x4 & mat)
{
	return XMFLOAT4X4(mat.a1,mat.a2,mat.a3,mat.a4,
		mat.b1,mat.b2,mat.b3,mat.b4,
		mat.c1,mat.c2,mat.c3,mat.c4,
		mat.d1,mat.d2,mat.d3,mat.d4);
}
