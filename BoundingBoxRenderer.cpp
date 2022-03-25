#include "BoundingBoxRenderer.h"

bool BoundingBoxRenderer::SetObject(GameObject * gameObj)
{
	if (gameObj == nullptr)
		return false;

	MeshRenderer* renderer = gameObj->GetComponent<MeshRenderer>();
	SkinnedMeshRenderer* skinnedRenderer = gameObj->GetComponent<SkinnedMeshRenderer>();
	

	//Object�� Renderer�� ������ return
	if (renderer != nullptr)
		m_meshOfCurrentObj = renderer->GetMesh();
	else if (skinnedRenderer != nullptr)
		m_meshOfCurrentObj = skinnedRenderer->GetMesh();
	else
	{
		m_obj = nullptr;
		m_meshOfCurrentObj = nullptr;
		return false;
	}
		

	if (mesh == nullptr)
	{
		mesh = new Mesh("BoundingBoxMesh");
		UINT idx[24] = { 0,1,0,4,4,5,1,5,0,2,4,6,5,7,1,3,2,3,2,6,6,7,3,7 };

		vector<UINT> indices(idx, idx + 24);

		mesh->Init(AABBtoVertices(m_meshOfCurrentObj->GetAABB()), indices);
		mesh->InitWritableVB<MyVertex::BasicVertex>(m_device, mesh->vertices.size());
		mesh->InitIB(m_device);
	}
	
	m_obj = gameObj;
	SetTransform(&gameObj->transform);

	VertexUpdate();

	return true;
}

vector<MyVertex::BasicVertex> BoundingBoxRenderer::AABBtoVertices(XNA::AxisAlignedBox aabb)
{
	int multiply[2] = { 1,-1 };
	vector<MyVertex::BasicVertex> vertices;

	//aabb�κ��� �������� ��ġ�� ����.
	for (int i = 0; i < 2; i++)
	{
		float x = aabb.Center.x;

		x += aabb.Extents.x * multiply[i];

		for (int i = 0; i < 2; i++)
		{
			float y = aabb.Center.y;
			y += aabb.Extents.y * multiply[i];

			for (int i = 0; i < 2; i++)
			{
				float z = aabb.Center.z;
				z += aabb.Extents.z * multiply[i];
				vertices.push_back({ x,y,z });
			}
		}
	}

	return vertices;
}

void BoundingBoxRenderer::PipeLineSetting()
{
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

}

void BoundingBoxRenderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (mesh == nullptr)
		return;
	else if (m_obj == nullptr)
		return;

	XMMATRIX world = XMLoadFloat4x4(&transform->m_world);

	//��������, �ε������۸� �Է������⿡ ����
	mesh->SetVB(context);
	mesh->SetIB(context);

	int subsetLength = mesh->GetSubsetLength();

	ID3DX11EffectTechnique* activeTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	for (int i = 0; i < subsetLength; i++)
	{
		//���̴� ���� ���������� ����
		if (i == 0 || (effects[i - 1] != effects[i])) //�������� ���� ���̴��� ������������
		{
			//�Է������� ����
			if (!effects[i]->IASetting(context, m_technique_type))
				break;
			//��º��ձ� ����
			if(!effects[i]->OMSetting(context, m_blending))
				break;
		}
		//���ϱ��� line���� ����
		PipeLineSetting();
		
		activeTech = effects[i]->GetTechnique(m_technique_type);

		activeTech->GetDesc(&techDesc);



		//shader�� �ʿ��� ������ ����
		effects[i]->PerObjectSet(&materials[i],
			camera, world);


		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			activeTech->GetPassByIndex(p)->Apply(0, context);
			mesh->Draw(context, i);
		}
	}
}
void BoundingBoxRenderer::InitEffects()
{
	effects.clear();
	Effect* effect = m_effectMgr.GetEffect(L"FX/Basic.fxo");
	effects.push_back(effect);
}


BoundingBoxRenderer::BoundingBoxRenderer(ID3D11Device * device, ID3D11DeviceContext * context)
	: Renderer("BoundingBoxRenderer", ComponentType::MESHRENDERER)
	, m_device(device), m_context(context), m_meshOfCurrentObj(0)
{
	std::vector<GeneralMaterial> tempMaterials(1);
	SetMaterials(tempMaterials);
	InitEffects();
	
}

BoundingBoxRenderer::~BoundingBoxRenderer()
{
	if (mesh != nullptr)
		delete mesh;
	mesh = nullptr;
}

void BoundingBoxRenderer::VertexUpdate()
{
	if (m_obj == nullptr || m_meshOfCurrentObj == nullptr)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_context->Map(mesh->GetVB(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));


	MyVertex::BasicVertex* v = reinterpret_cast<MyVertex::BasicVertex*>(mappedData.pData);
	std::vector<MyVertex::BasicVertex> tempVertices = AABBtoVertices(m_meshOfCurrentObj->GetAABB());

	for (UINT i = 0; i < tempVertices.size(); ++i)
	{
		v[i] = tempVertices[i];
	}

	m_context->Unmap(mesh->GetVB(), 0);
}
