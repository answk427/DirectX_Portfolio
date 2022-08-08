#include "AssimpLoader.h"
#include <atlconv.h>


void AssimpLoader::InitScene(const std::string& fileName)
{
	m_pScene = aiImportFile(fileName.c_str(),
		aiProcessPreset_TargetRealtime_Quality |
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
		boneHierarchy.Init();
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
	boneHierarchy.InitBones(root, m_assimpBones);
	boneHierarchy.SortBones();
	if (m_pScene->HasAnimations())
	{
		m_animations.clear();
		for (int i = 0; i < m_pScene->mNumAnimations; ++i)
		{
			LoadAnimation(m_pScene->mAnimations[i]); 
		}
	}
	for(auto& animation : m_animations)
		boneHierarchy.InitAnimation(animation.second);
	
	boneHierarchy.ConvertSkinnedVertex(root);
	
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
	USES_CONVERSION;
	std::string title = ExtractTitle(currentFileName) + "_" + animation->mName.C_Str();
	m_animations[title] = AssimpAnimation();
	
	AssimpAnimation& anim = m_animations[title];

	anim.ticksPerSecond = animation->mTicksPerSecond;
	//anim.duration = animation->mDuration;
	anim.duration = animation->mDuration / anim.ticksPerSecond;
	
	//channelName = boneName = nodeName
	boneName channelName;
	AssimpAnimationFrame* currBone;
	aiNodeAnim* nodeAnim;
	
	aiVectorKey* currKeyPos;
	aiQuatKey* currKeyRot;
	aiVectorKey* currKeyScale;
	for (int i = 0; i < animation->mNumChannels; ++i)
	{
		nodeAnim= animation->mChannels[i];
		channelName = A2W(nodeAnim->mNodeName.C_Str());
		
		if (anim.bones.find(channelName) == anim.bones.end())
		{
			const auto&[it, success] = anim.bones.insert({ channelName, AssimpAnimationFrame(channelName) });
			currBone = &(it->second);
		}
		else
			currBone = &anim.bones[channelName];
				
		for (int j = 0; j < nodeAnim->mNumPositionKeys; ++j)
		{
			currKeyPos = &nodeAnim->mPositionKeys[j];
			currBone->positionKey.push_back({ currKeyPos->mTime / anim.ticksPerSecond, currKeyPos->mValue.x, currKeyPos->mValue.y, currKeyPos->mValue.z });
		}
		for (int j = 0; j < nodeAnim->mNumRotationKeys; ++j)
		{
			currKeyRot = &nodeAnim->mRotationKeys[j];
			currBone->quaternionKey.push_back({ currKeyRot->mTime / anim.ticksPerSecond, currKeyRot->mValue.x, currKeyRot->mValue.y, currKeyRot->mValue.z, currKeyRot->mValue.w });

		}
		for (int j = 0; j < nodeAnim->mNumScalingKeys; ++j)
		{
			currKeyScale = &nodeAnim->mScalingKeys[j];
			currBone->scalingKey.push_back({ currKeyScale->mTime / anim.ticksPerSecond, currKeyScale->mValue.x, currKeyScale->mValue.y, currKeyScale->mValue.z });
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
	USES_CONVERSION;

	MyVertex::Subset tempSubset;
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
		std::wstring nodeName;
		
		
		std::vector<std::wstring> tempBoneNames;
		for (int i = 0; i < mesh->mNumBones; ++i)
		{			
			currBone = mesh->mBones[i];
			
			//���� �̸��� wstring���� ��ȯ
			{nodeName = A2W(currBone->mName.C_Str()); }
			tempBoneNames.push_back(nodeName);

			//bone ������ offset��ȯ��� �߰�
			m_assimpBones[nodeName].offsetMat = ConvertMatrix(currBone->mOffsetMatrix);
			m_assimpBones[nodeName].toParentMat = ConvertMatrix(currBone->mNode->mTransformation);
			//m_assimpBones[nodeName].toParentMat = ConvertMatrix(currBone->mArmature->mTransformation);
			
			int currVertexId;
			bool repeated;
			for (int j = 0; j < currBone->mNumWeights; ++j)
			{
				currVertexId = tempSubset.VertexStart +
					currBone->mWeights[j].mVertexId;
								
				currSkinnedVertex = &assimpMesh.skinnedVertices[currVertexId];
				
				repeated = false;
				//�ߺ����� ���� ��� ����
				for (auto& vertexNodeName : currSkinnedVertex->nodeName)
				{
					if (vertexNodeName == nodeName)
					{
						repeated = true;
						break;
					}
				}
				if (repeated)
					continue;
				
				//vertexId��° ����ġ ���� �߰�
				currSkinnedVertex->weights.push_back(currBone->mWeights[j].mWeight);
				
				//����ġ�� ������ ��(���)�� �̸� �߰�
				currSkinnedVertex->nodeName.push_back(nodeName);			
			}
		}
		boneNames.push_back(tempBoneNames);
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
	/*return XMFLOAT4X4(mat.a1,mat.a2,mat.a3,mat.a4,
		mat.b1,mat.b2,mat.b3,mat.b4,
		mat.c1,mat.c2,mat.c3,mat.c4,
		mat.d1,mat.d2,mat.d3,mat.d4);*/
	XMFLOAT4X4 resultMat;
	XMStoreFloat4x4(&resultMat,
		XMMatrixTranspose(XMMATRIX(
			mat.a1, mat.a2, mat.a3, mat.a4,
		mat.b1, mat.b2, mat.b3, mat.b4,
		mat.c1, mat.c2, mat.c3, mat.c4,
		mat.d1, mat.d2, mat.d3, mat.d4)));
	return resultMat;
}

void FinalHierarchy::Init()
{
	m_boneNameIdx.clear();
	parents.clear();
	offsets.clear();
	boneParentMatrix.clear();
}

void FinalHierarchy::InitBones(NodeStruct * root, std::map<boneName, AssimpBone>& assimpBones)
{
	if (root == nullptr)
		return;
	if (assimpBones.empty())
		return;
	
	//for (auto& animation : animations)
	//{
	//	for (auto& bone : animation.second.bones)
	//	{
	//		//���ο� �� �̸��� �� ������ �߰�
	//		if (assimpBones.find(bone.first) == tempBones.end())
	//		{
	//			//�� �������� ���� Animation���� �ִ� ������ ������ķ� �ʱ�ȭ
	//			tempBones[bone.first] = { identity };
	//		}
	//	}
	//}

	parents.reserve(assimpBones.size());
	offsets.reserve(assimpBones.size());
	boneParentMatrix.reserve(assimpBones.size());
	
	std::queue<std::pair<int, NodeStruct*>> q;
	int parentIdx = -1;
	auto it = assimpBones.find(root->GetName());
	
	//root ��尡 ���� ���? ������ �𸣰���
	if (it != assimpBones.end())
	{ 
		parentIdx = 0;
		m_boneNameIdx[it->first] = 0;
		parents.push_back(-1); //0�� ��Ʈ�� �θ�� -1
		offsets.push_back(it->second.offsetMat);
	}
		
	q.push({ parentIdx, root });

	NodeStruct* currNode;
	//��� ��带 bfs�� Ž���ϸ鼭 BoneName�� ��ġ�ϴ� ��尡 ������ �� ���� �߰�
	while (!q.empty())
	{
		parentIdx = q.front().first;
		currNode = q.front().second;
		q.pop();

		it = assimpBones.find(currNode->GetName());
		//���� ��尡 Bone Name�� ��ġ�� ��
		if (it != assimpBones.end())
		{
			parents.push_back(parentIdx);
			offsets.push_back(it->second.offsetMat);
			boneParentMatrix.push_back(it->second.toParentMat);
			
			m_boneNameIdx[it->first] = parents.size() - 1;
			parentIdx = parents.size() - 1;
		}

		for (auto& child : currNode->childs)
		{
			q.push({ parentIdx, &child });
		}
	}

}

void FinalHierarchy::SortBones()
{
	for (auto bone : m_boneNameIdx)
	{
		m_sortBones.insert({ bone.second, bone.first });
	}
}

void FinalHierarchy::InitAnimation(AssimpAnimation & animation)
{
	animation.HierarchyInit(m_boneNameIdx.size());

	for (auto& bone : animation.bones)
	{
		auto it = m_boneNameIdx.find(bone.first);
		if (it != m_boneNameIdx.end()) 
		{
			//�������� index�� ��ȯ�ؼ� �������� ������ �ִϸ��̼��� vector�� ���ġ
			animation.HierarchyAniClip[it->second] = bone.second;
		}
	}
}

void FinalHierarchy::ConvertSkinnedVertex(AssimpSkinnedVertex & vertex)
{
	vertex.nodeIndices.clear();
	for (auto& nodeName : vertex.nodeName)
	{
		auto it = m_boneNameIdx.find(nodeName);
		if (it != m_boneNameIdx.end())
		{
			//boneName�� �������������� �ε����� ��ȯ
			vertex.nodeIndices.push_back(it->second);
		}
	}
}

void FinalHierarchy::ConvertSkinnedVertex(NodeStruct * root)
{
	if (root == nullptr)
		return;

	if (root->assimpMesh != nullptr)
	{
		for (int i=0; i< root->assimpMesh->skinnedVertices.size(); ++i)
		{
			auto& skinnedVertex = root->assimpMesh->skinnedVertices[i];
			ConvertSkinnedVertex(skinnedVertex);
			float result = 0.0f;
			for (auto weight : skinnedVertex.weights)
			{
				result += weight;	
			}
			
		}
	}

	
	for (auto& child : root->childs)
	{
		ConvertSkinnedVertex(&child);
	}

}

void AssimpAnimationFrame::resizeKeys(int size)
{
	positionKey.resize(size, { 0.0,0.0f,0.0f,0.0f });
	quaternionKey.resize(size, { 0.0,0.0f,0.0f,0.0f,1.0f });
	scalingKey.resize(size, { 0.0,0.0f,0.0f,0.0f });
}

