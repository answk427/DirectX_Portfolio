#include "AssimpLoader.h"
#include <atlconv.h>


void AssimpLoader::InitScene(const string& fileName)
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
		//멤버 컨테니어 초기화
		InitContainer();
	}
	
	
	
}

void AssimpLoader::InitContainer()
{
	//전부 비우는 작업
	vector<MyVertex::BasicVertex>().swap(vertices);
	vector<UINT>().swap(indices);
	vector<Subset>().swap(subsets);
	vector<GeneralMaterial>().swap(materials);
}

bool AssimpLoader::LoadData()
{
	if (m_pScene == NULL)
		return false;

	NodeTravel(m_pScene->mRootNode);

	return true;
}


void AssimpLoader::SetMaterial(int matNumOfMesh)
{

	aiMaterial* aiMat = m_pScene->mMaterials[matNumOfMesh];
	GeneralMaterial tempMaterial;
	
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

	
	//A2W(유니코드 -> 멀티바이트 변환 함수)를 사용하기위한 매크로
	USES_CONVERSION;
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

	materials.push_back(tempMaterial);
}



void AssimpLoader::NodeTravel(aiNode * node)
{
	//해당 노드의 매쉬를 적재
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		SetMesh(mesh);
	}

	//모든 자식노드를 순회
	for (int i = 0; i < node->mNumChildren; i++)
		NodeTravel(node->mChildren[i]);

	return;
}

void AssimpLoader::SetMesh(aiMesh * mesh)
{
	Subset tempSubset;
	//material인덱스 
	tempSubset.materialNum = mesh->mMaterialIndex;
	//현재 vertex, index 갯수가 이 다음 subset의 시작점
	tempSubset.VertexStart = vertexCount;
	tempSubset.IndexStart = indexCount;

	//총 정점의 갯수를 구함
	vertexCount += mesh->mNumVertices;
	vertices.reserve(vertexCount + 10);

	//index의 총 갯수
	indexCount += mesh->mNumFaces * 3;
	indices.reserve(indexCount + 10);

	tempSubset.VertexCount = mesh->mNumVertices;
	tempSubset.IndexCount = mesh->mNumFaces * 3;

	subsets.push_back(tempSubset);

	//Material 데이터 적재
	SetMaterial(mesh->mMaterialIndex);

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
		vertices.push_back(tempVertex);
	}


	//index 적재
	if (mesh->HasFaces())
	{
		//삼각형으로 이루어진 면들의 인덱스 적재
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
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
