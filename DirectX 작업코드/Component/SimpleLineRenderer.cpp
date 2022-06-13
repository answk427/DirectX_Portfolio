#include "SimpleLineRenderer.h"

void SimpleLineRenderer::AddBoundingBox(const XNA::AxisAlignedBox & aabb)
{
	std::vector<MyVertex::PosColorVertex> tempVertices = AABBtoVertices(aabb);
	UINT startVertex = m_vertices.size();
	for (auto& vertex : tempVertices)
		m_vertices.push_back(vertex);

	
	UINT idxOffset[24] = { 0,1,0,4,4,5,1,5,0,2,4,6,5,7,1,3,2,3,2,6,6,7,3,7 };
	for (int i = 0; i < 24; ++i)
		mesh->indices.push_back(startVertex + idxOffset[i]);

	mesh->subsets[0].IndexCount = mesh->indices.size();
	mesh->InitVB(m_texMgr.md3dDevice, m_vertices);
	mesh->InitIB(m_texMgr.md3dDevice);
	
	return;
}

void SimpleLineRenderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (m_vertices.empty() || mesh==nullptr)
		return;

	XMMATRIX world = XMLoadFloat4x4(&transform->m_world);

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
			if (!effects[i]->OMSetting(context, m_blending))
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