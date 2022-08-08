#include "AssimpLoader.h"
#include <atlconv.h>


void AssimpLoader::InitScene(const std::string& fileName)
{
	m_pScene = aiImportFile(fileName.c_str(),
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_GenBoundingBoxes | //BoundingBox 계산?

		aiProcess_CalcTangentSpace | //탄젠트공간 좌표축 계산 tangent, bitangent

		aiProcess_ConvertToLeftHanded | //왼손좌표계로 변환

		aiProcess_Triangulate | //매쉬를 구성하는 다각형 면을 삼각형으로 만듬

		aiProcess_GenSmoothNormals | //부드러운 normal벡터 생성

		aiProcess_SplitLargeMeshes | //매쉬를 분할

		aiProcess_LimitBoneWeights | //정점에 영향을 주는 뼈의 개수 4개로 제한

		aiProcess_ValidateDataStructure | //데이터 구조의 유효성을 검사

		aiProcess_ImproveCacheLocality | //캐시 최적화

		aiProcess_RemoveRedundantMaterials | //중복 메테리얼 제거

		aiProcess_FixInfacingNormals | //안쪽을 향하는 normal벡터 반전

		aiProcess_PopulateArmatureData | //뼈대관련 플레그 일단 여러개의 뼈대있을때 활성화

		aiProcess_SortByPType | //단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬

		aiProcess_FindInstances  //중복 메쉬 제거
		
	);

	assert(m_pScene);

	if (m_pScene) //경로의 파일을 정상적으로 로드했을 때
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
//	//전부 비우는 작업		
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

	//A2W(유니코드 -> 멀티바이트 변환 함수)를 사용하기위한 매크로
	USES_CONVERSION;
	//Material name
	aiString& aiName = aiMat->GetName();
	tempMaterial.name = std::wstring(A2W(aiName.C_Str()));
	
	aiColor4D color;

	//Material의 Diffuse 값 적재
	aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	tempMaterial.basicMat.Diffuse = { color.r, color.g, color.b, 1.0f };
	color = { 0,0,0,0 };

	//Material의 ambient 값 적재
	aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	tempMaterial.basicMat.Ambient = { color.r, color.g, color.b, 1.0f };
	color = { 0,0,0,0 };

	//Material의 Specular 값 적재
	aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	tempMaterial.basicMat.Specular = { color.r, color.g, color.b, color.a };
	color = { 0,0,0,0 };

	//Material의 Reflect 값 적재
	aiMat->Get(AI_MATKEY_COLOR_REFLECTIVE, color);
	tempMaterial.basicMat.Reflect = { color.r, color.g, color.b, color.a };

	//*************Texture 적재 **************//
	aiString fileName;

	
	
	//DiffuseTexture 경로 저장
	aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &fileName);
	tempMaterial.diffuseMapName = A2W(fileName.C_Str());
	fileName.Clear();


	//SpecularTexture 경로 저장
	aiMat->GetTexture(aiTextureType_SPECULAR, 0, &fileName);
	tempMaterial.specularMap = A2W(fileName.C_Str());
	fileName.Clear();

	//NormalTexture 경로 저장
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
	
	//변환행렬 적재
	root = new NodeStruct(std::wstring(A2W(node->mName.C_Str()))
		, ConvertMatrix(node->mTransformation));
	
	root->GetMatrix();
	//해당 노드의 매쉬를 적재
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		if (root->assimpMesh == nullptr)
			root->assimpMesh = new AssimpMesh();
		SetMesh(*(root->assimpMesh),mesh);
	}
	
	//모든 자식노드를 순회
	for (int i = 0; i < node->mNumChildren; i++)
		NodeTravel(*root, node->mChildren[i]);
	
}

void AssimpLoader::NodeTravel(NodeStruct& parent ,aiNode * node)
{
	USES_CONVERSION;
	
	
	parent.childs.push_back(NodeStruct(std::wstring(A2W(node->mName.C_Str())),
		ConvertMatrix(node->mTransformation)));
	NodeStruct& current = parent.childs.back();
	
	//해당 노드의 매쉬를 적재
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		if (current.assimpMesh == nullptr)
			current.assimpMesh = new AssimpMesh();
		SetMesh(*(current.assimpMesh),mesh);
	}

	//모든 자식노드를 순회
	for (int i = 0; i < node->mNumChildren; i++)
		NodeTravel(current,node->mChildren[i]);

	return;
}

void AssimpLoader::SetMesh(AssimpMesh& assimpMesh, aiMesh * mesh)
{
	USES_CONVERSION;

	MyVertex::Subset tempSubset;
	//material인덱스 
	tempSubset.materialNum = mesh->mMaterialIndex;

	//현재 vertex, index 갯수가 이 다음 subset의 시작점
	tempSubset.VertexStart = assimpMesh.vertexCount;
	tempSubset.IndexStart = assimpMesh.indexCount;

	//총 정점의 갯수를 구함
	assimpMesh.vertexCount += mesh->mNumVertices;
	assimpMesh.vertices.reserve(assimpMesh.vertexCount);
	assimpMesh.skinnedVertices.resize(assimpMesh.vertexCount);

	//index의 총 갯수
	assimpMesh.indexCount += mesh->mNumFaces * 3;
	assimpMesh.indices.reserve(assimpMesh.indexCount);

	tempSubset.VertexCount = mesh->mNumVertices;
	tempSubset.IndexCount = mesh->mNumFaces * 3;

	assimpMesh.subsets.push_back(tempSubset);

	//Material 데이터 적재
	assimpMesh.materials.push_back(SetMaterial(mesh->mMaterialIndex));
	

	
	//AABB BoundingBox 적재
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
			
			//뼈의 이름을 wstring으로 변환
			{nodeName = A2W(currBone->mName.C_Str()); }
			tempBoneNames.push_back(nodeName);

			//bone 정보에 offset변환행렬 추가
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
				//중복으로 들어가는 경우 방지
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
				
				//vertexId번째 가중치 정보 추가
				currSkinnedVertex->weights.push_back(currBone->mWeights[j].mWeight);
				
				//가중치와 연관된 뼈(노드)의 이름 추가
				currSkinnedVertex->nodeName.push_back(nodeName);			
			}
		}
		boneNames.push_back(tempBoneNames);
	}

	if (assimpMesh.m_AABB_MaxMin == nullptr)
		assimpMesh.m_AABB_MaxMin = new AABB_MaxMin(tempAabbMax, tempAabbMin);
	else
	{
		//현재 바운딩박스와 비교
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

	//정점 구조체 데이터
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		MyVertex::BasicVertex tempVertex;

		//position 좌표 적재
		if (mesh->HasPositions())
		{
			aiVector3D& position = mesh->mVertices[i];
			tempVertex.pos = XMFLOAT3(position.x, position.y, position.z);
		}

		//Texture 좌표 적재
		if (mesh->HasTextureCoords(0))
		{
			//배열의 주소로 매개변수 넘김
			tempVertex.tex = XMFLOAT2(&mesh->mTextureCoords[0][i].x);
		}

		//법선벡터 적재
		if (mesh->HasNormals())
		{
			aiVector3D& normal = mesh->mNormals[i];
			tempVertex.normal = XMFLOAT3(normal.x, normal.y, normal.z);
		}

		//접선벡터 적재
		if (mesh->HasTangentsAndBitangents())
		{
			aiVector3D& tangent = mesh->mTangents[i];
			aiVector3D& biTangent = mesh->mBitangents[i];

			tempVertex.tan = XMFLOAT3(tangent.x, tangent.y, tangent.z);
			tempVertex.biTan = XMFLOAT3(biTangent.x, biTangent.y, biTangent.z);
		}
		assimpMesh.vertices.push_back(tempVertex);
	}


	//index 적재
	if (mesh->HasFaces())
	{
		//삼각형으로 이루어진 면들의 인덱스 적재
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
	//		//새로운 뼈 이름일 때 데이터 추가
	//		if (assimpBones.find(bone.first) == tempBones.end())
	//		{
	//			//뼈 정보에는 없이 Animation에만 있던 정보라 단위행렬로 초기화
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
	
	//root 노드가 뼈인 경우? 있을지 모르겠음
	if (it != assimpBones.end())
	{ 
		parentIdx = 0;
		m_boneNameIdx[it->first] = 0;
		parents.push_back(-1); //0번 루트의 부모는 -1
		offsets.push_back(it->second.offsetMat);
	}
		
	q.push({ parentIdx, root });

	NodeStruct* currNode;
	//모든 노드를 bfs로 탐색하면서 BoneName과 일치하는 노드가 있으면 뼈 정보 추가
	while (!q.empty())
	{
		parentIdx = q.front().first;
		currNode = q.front().second;
		q.pop();

		it = assimpBones.find(currNode->GetName());
		//현재 노드가 Bone Name과 일치할 때
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
			//노드네임을 index로 변환해서 계층구조 순서로 애니메이션을 vector에 재배치
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
			//boneName을 계층구조에서의 인덱스로 변환
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

