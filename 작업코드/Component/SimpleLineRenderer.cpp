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

	//��������, �ε������۸� �Է������⿡ ����
	SetVB(context);
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
			if (!effects[i]->OMSetting(context, m_blending))
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