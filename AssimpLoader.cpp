#include "AssimpLoader.h"
#include <atlconv.h>


void AssimpLoader::InitScene(const std::string& fileName)
{
	m_pScene = aiImportFile(fileName.c_str(),
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
	
	Subset tempSubset;
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
		for (int i = 0; i < mesh->mNumBones; ++i)
		{			
			currBone = mesh->mBones[i];
			if (currBone->mWeights->mVertexId >= assimpMesh.skinnedVertices.size() ||
				currBone->mWeights->mWeight <= 0)
				continue;
			currSkinnedVertex = &assimpMesh.skinnedVertices[currBone->mWeights->mVertexId];
			
			//vertexId번째 가중치 정보 추가
			currSkinnedVertex->weights.push_back(currBone->mWeights->mWeight);
			
			
			//가중치와 연관된 뼈(노드)의 이름 추가
			std::string nodeName = currBone->mName.C_Str();
			currSkinnedVertex->nodeName.push_back(nodeName);
				
			//bone 정보에 offset변환행렬 추가
			m_assimpBones[nodeName].offsetMat = ConvertMatrix(currBone->mOffsetMatrix);
		}
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
	return XMFLOAT4X4(mat.a1,mat.a2,mat.a3,mat.a4,
		mat.b1,mat.b2,mat.b3,mat.b4,
		mat.c1,mat.c2,mat.c3,mat.c4,
		mat.d1,mat.d2,mat.d3,mat.d4);
}
