#include "ObjectLoader.h"
#include <atlconv.h>


void ObjectLoader::InitScene(const string& fileName)
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

	//m_pScene = aiImportFile(fileName.c_str(), aiProcess_JoinIdenticalVertices |        // 동일한 꼭지점 결합, 인덱싱 최적화

	//	aiProcess_ValidateDataStructure |        // 로더의 출력을 검증

	//	aiProcess_ImproveCacheLocality |        // 출력 정점의 캐쉬위치를 개선

	//	aiProcess_RemoveRedundantMaterials |    // 중복된 매터리얼 제거

	//	aiProcess_GenUVCoords |                    // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환

	//	aiProcess_TransformUVCoords |            // UV 변환 처리기 (스케일링, 변환...)

	//	aiProcess_FindInstances |                // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거

	//	aiProcess_LimitBoneWeights |            // 정점당 뼈의 가중치를 최대 4개로 제한

	//	aiProcess_OptimizeMeshes |                // 가능한 경우 작은 매쉬를 조인

	//	aiProcess_GenSmoothNormals |            // 부드러운 노말벡터(법선벡터) 생성

	//	aiProcess_SplitLargeMeshes |            // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)

	//	aiProcess_Triangulate |                    // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)

	//	aiProcess_ConvertToLeftHanded |            // D3D의 왼손좌표계로 변환

	//	aiProcess_SortByPType);                    // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬

	if (m_pScene) //경로의 파일을 정상적으로 로드했을 때
	{
		
	}
	else
	{
		HR(-1);
	}
}

bool ObjectLoader::LoadData()
{
	//Init이 제대로 안된 상황
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
	//material의 개수만큼 초기화
	materials->assign(m_pScene->mNumMaterials, GeneralMaterial());
}

void ObjectLoader::SetMaterial(const int & matNumOfMesh)
{
	aiMaterial* aiMat = m_pScene->mMaterials[matNumOfMesh];
		
	aiColor4D color;
	
	//Material의 Diffuse 값 적재
	aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	(*materials)[matNumOfMesh].basicMat.Diffuse = { color.r, color.g, color.b, 1.0f };
	
	//Material의 ambient 값 적재
	aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	(*materials)[matNumOfMesh].basicMat.Ambient = { color.r, color.g, color.b, 1.0f };
	
	//Material의 Specular 값 적재
	aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	(*materials)[matNumOfMesh].basicMat.Specular = { color.r, color.g, color.b, color.a };

	//*************Texture 적재 **************//
	aiString* fileName;
	
	//A2W(유티코드 -> 멀티바이트 변환 함수)를 사용하기위한 매크로
	USES_CONVERSION; 

	//DiffuseTexture 경로 저장
	aiMat->GetTexture(aiTextureType_DIFFUSE, 0, fileName);
	(*materials)[matNumOfMesh].diffuseMapName = A2W(fileName->C_Str());
	
	
	//SpecularTexture 경로 저장
	aiMat->GetTexture(aiTextureType_SPECULAR, 0, fileName);
	(*materials)[matNumOfMesh].specularMap= A2W(fileName->C_Str());

	//NormalTexture 경로 저장
	aiMat->GetTexture(aiTextureType_NORMALS, 0, fileName);
	(*materials)[matNumOfMesh].normalMapName = A2W(fileName->C_Str());
}



void ObjectLoader::NodeTravel(aiNode * node)
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

void ObjectLoader::SetMesh(aiMesh * mesh)
{
	
	Subset tempSubset;
	//material인덱스 
	tempSubset.materialNum = mesh->mMaterialIndex;
	//현재 vertex, index 갯수가 이 다음 subset의 시작점
	tempSubset.VertexStart = vertexCount;
	tempSubset.IndexStart = indexCount;
	
	//총 정점의 갯수를 구함
	vertexCount += mesh->mNumVertices;
	vertices->reserve(vertexCount+10);

	//index의 총 갯수
	indexCount += mesh->mNumFaces*3;
	indices->reserve(indexCount + 10);

	tempSubset.VertexCount = mesh->mNumVertices;
	tempSubset.IndexCount = mesh->mNumFaces*3;
	
	subsets->push_back(tempSubset);
	
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
		vertices->push_back(tempVertex);
	}

	
	//index 적재
	if (mesh->HasFaces())
	{
		//삼각형으로 이루어진 면들의 인덱스 적재
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
