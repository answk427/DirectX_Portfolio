#include "ObjectLoader.h"

void ObjectLoader::InitScene()
{
	if (m_pScene->HasMeshes()) //mesh가 로드 되었을 때
	{
		for (int i = 0; i < m_pScene->mNumMeshes; i++)
		{
			const aiMesh* tempMesh = m_pScene->mMeshes[i];
			//InitMesh(i, tempMesh);
		}
	}
	
}

//void ObjectLoader::InitMesh(UINT index, const aiMesh * pMesh, vector<)
//{
//	m_meshes[index].vertices.reserve(pMesh->mNumVertices);
//	m_meshes[index].indices.reserve(pMesh->mNumFaces * 3);
//	m_numVertex += pMesh->mNumVertices;
//	m_numIndex += pMesh->mNumFaces * 3;
//	
//
//	//정점 초기화
//	for (int i = 0; i < pMesh->mNumVertices; i++)
//	{
//		//XMFLOAT3(float *pArray) 초기화
//		XMFLOAT3 pos(&pMesh->mVertices[i].x);
//		XMFLOAT3 normal(&pMesh->mNormals[i].x);
//		XMFLOAT2 tex;
//		
//		if (pMesh->HasTextureCoords(0)) //텍스쳐좌표set가 있는지 확인
//			tex = XMFLOAT2(&pMesh->mTextureCoords[0][i].x);
//		else
//			tex = XMFLOAT2(0.0f, 0.0f);
//
//		Vertex::Basic32 vertex = { pos, normal, tex };
//		m_meshes[index].vertices.push_back(vertex);
//		vertices.push_back(vertex);
//
//	}
//	
//	//index 초기화
//	if (pMesh->HasFaces()) 
//	{
//		for (int i = 0; i < pMesh->mNumFaces; i++)
//		{
//			const aiFace& face = pMesh->mFaces[i];
//			m_meshes[index].indices.push_back(face.mIndices[0]);
//			m_meshes[index].indices.push_back(face.mIndices[1]);
//			m_meshes[index].indices.push_back(face.mIndices[2]);
//
//			indices.push_back(face.mIndices[0]);
//			indices.push_back(face.mIndices[1]);
//			indices.push_back(face.mIndices[2]);
//		}
//	}
//
//	
//
//}
//
//void ObjectLoader::setVB(ID3D11Device* device)
//{
//	ReleaseCOM(m_VB);
//
//	D3D11_BUFFER_DESC buffer_desc;
//
//	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
//	buffer_desc.ByteWidth = sizeof(Vertex::Basic32) * m_numVertex; 
//	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	buffer_desc.CPUAccessFlags = 0;
//	buffer_desc.MiscFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA vinitData;
//	vinitData.pSysMem = &vertices[0];
//	HR(device->CreateBuffer(&buffer_desc, &vinitData, &m_VB));
//
//}
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
//void ObjectLoader::setIB(ID3D11Device* device)
//{
//	ReleaseCOM(m_IB);
//	D3D11_BUFFER_DESC buffer_desc;
//	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
//	buffer_desc.ByteWidth = sizeof(UINT32) * m_numIndex;
//	buffer_desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
//	buffer_desc.CPUAccessFlags = 0;
//	buffer_desc.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA vinitData;
//	vinitData.pSysMem = &indices[0];
//	HR(device->CreateBuffer(&buffer_desc, &vinitData, &m_IB));
//	
//}
//
//void ObjectLoader::Draw(ID3D11DeviceContext * context)
//{
//	UINT stride = sizeof(Vertex::Basic32);
//	UINT offset = 0;
//	context->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
//	
//	context->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
//	
//	
//}

ObjectLoader::ObjectLoader(const string & fileName) //: m_VB(0), m_IB(0), m_numBones(0), m_numIndex(0), m_numMaterial(0), m_numVertex(0)
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
		InitScene();
		
	}
	else
	{
		HR(-1);
	}
}
