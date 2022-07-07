#include "BoundingBoxRenderer.h"

bool BoundingBoxRenderer::SetObject(GameObject * gameObj)
{
	if (gameObj == nullptr)
		return false;

	MeshRenderer* renderer = dynamic_cast<MeshRenderer*>(gameObj->GetComponent(ComponentType::MESHRENDERER));
	SkinnedMeshRenderer* skinnedRenderer = dynamic_cast<SkinnedMeshRenderer*>(gameObj->GetComponent(ComponentType::SKINNEDMESHRENDERER));
	

	//Object에 Renderer가 없으면 return
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

		std::vector<UINT> indices(idx, idx + 24);

		mesh->Init(std::vector<MyVertex::BasicVertex>(), indices);
		mesh->InitWritableVB<MyVertex::PosColorVertex>(m_device, 8);
		mesh->InitIB(m_device);
	}
	
	m_obj = gameObj;
	SetTransform(gameObj->transform.get());
	SetNodeHierarchy(gameObj->nodeHierarchy);
	ownerObjectId = gameObj->GetID();
	
	VertexUpdate();

	return true;
}

std::vector<MyVertex::PosColorVertex> BoundingBoxRenderer::AABBtoVertices(XNA::AxisAlignedBox aabb)
{
	int multiply[2] = { 1,-1 };
	std::vector<MyVertex::PosColorVertex> vertices;
	
	//aabb로부터 정점들의 위치를 구함.
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

	//XMMATRIX world = XMLoadFloat4x4(&transform->m_world);
	XMMATRIX world;
	m_bones.lock()->GetFinalTransform(world, ownerObjectId);
	
	//정점버퍼, 인덱스버퍼를 입력조립기에 묶음
	SetVB(context);
	mesh->SetIB(context);

	int subsetLength = mesh->GetSubsetLength();

	ID3DX11EffectTechnique* activeTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	for (int i = 0; i < subsetLength; i++)
	{
		//쉐이더 별로 파이프라인 설정
		if (i == 0 || (effects[i - 1] != effects[i])) //연속으로 같은 쉐이더는 설정하지않음
		{
			//입력조립기 세팅
			if (!effects[i]->IASetting(context, m_technique_type))
				break;
			//출력병합기 세팅
			if(!effects[i]->OMSetting(context, m_blending))
				break;
		}
		//기하구조 line으로 변경
		PipeLineSetting();
		
		activeTech = effects[i]->GetTechnique(m_technique_type);

		activeTech->GetDesc(&techDesc);



		//shader에 필요한 데이터 설정
		effects[i]->PerObjectSet(&materials[i],
			camera, world);


		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			activeTech->GetPassByIndex(p)->Apply(0, context);
			mesh->Draw(context, i);
		}
	}
}
void BoundingBoxRenderer::SetVB(ID3D11DeviceContext * context)
{
	UINT stride[1] = { sizeof(MyVertex::PosColorVertex) };
	UINT offset[1] = { 0 };
	ID3D11Buffer* vb = mesh->GetVB();
	context->IASetVertexBuffers(0, 1, &vb, stride, offset);
}
void BoundingBoxRenderer::InitEffects()
{
	effects.clear();
	Effect* effect = new SimpleLineEffect(TextureMgr::Instance().md3dDevice, L"FX/SimpleLine.fxo");
	effects.push_back(effect);
}


BoundingBoxRenderer::BoundingBoxRenderer(ID3D11Device * device, ID3D11DeviceContext * context)
	: Renderer("BoundingBoxRenderer", ComponentType::MESHRENDERER, "BoundingBox")
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


	MyVertex::PosColorVertex* v = reinterpret_cast<MyVertex::PosColorVertex*>(mappedData.pData);
	std::vector<MyVertex::PosColorVertex> tempVertices = AABBtoVertices(m_meshOfCurrentObj->GetAABB());

	for (UINT i = 0; i < tempVertices.size(); ++i)
	{
		v[i] = tempVertices[i];
	}

	m_context->Unmap(mesh->GetVB(), 0);
}
