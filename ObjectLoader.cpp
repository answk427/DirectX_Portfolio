#include "ObjectLoader.h"

void ObjectLoader::InitScene()
{
	if (m_pScene->HasMeshes()) //mesh�� �ε� �Ǿ��� ��
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
//	//���� �ʱ�ȭ
//	for (int i = 0; i < pMesh->mNumVertices; i++)
//	{
//		//XMFLOAT3(float *pArray) �ʱ�ȭ
//		XMFLOAT3 pos(&pMesh->mVertices[i].x);
//		XMFLOAT3 normal(&pMesh->mNormals[i].x);
//		XMFLOAT2 tex;
//		
//		if (pMesh->HasTextureCoords(0)) //�ؽ�����ǥset�� �ִ��� Ȯ��
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
//	//index �ʱ�ȭ
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
		InitScene();
		
	}
	else
	{
		HR(-1);
	}
}
