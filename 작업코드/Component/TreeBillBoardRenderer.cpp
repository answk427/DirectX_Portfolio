#include "TreeBillBoardRenderer.h"

TreeBillBoardRenderer::TreeBillBoardRenderer() : Renderer("TreeBillBoardRenderer", ComponentType::MESHRENDERER)
{
	TextureMgr& texMgr = TextureMgr::Instance();

	//BillBoard �޽� ����
	mesh = new Mesh("TreeBillBoardMesh");
	mesh->Init(std::vector<MyVertex::BasicVertex>(),
		std::vector<UINT>());
	mesh->InitVB<TreePointSprite>(texMgr.md3dDevice, treeVertices);
	mesh->InitIB(texMgr.md3dDevice);
			
	//texture �߰�
	treeTextureNames.push_back(L"Textures/tree0.dds");
	treeTextureNames.push_back(L"Textures/tree1.dds");
	treeTextureNames.push_back(L"Textures/tree2.dds");
	treeTextureNames.push_back(L"Textures/tree3.dds");
	
	//subset1��, �⺻ material 1�� ���
	std::vector<GeneralMaterial> treeMaterials(1);
	treeMaterials[0].basicMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	treeMaterials[0].basicMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	treeMaterials[0].basicMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	SetMaterials(treeMaterials);

	InitEffects();

	SetTransform(new Transform(nullptr));

	SetTechniqueType(TechniqueType::Light | TechniqueType::DiffuseMap);
}

TreeBillBoardRenderer::~TreeBillBoardRenderer()
{
	if (mesh != nullptr)
		delete mesh;
	mesh = nullptr;
	if (transform != nullptr)
		delete transform;
}

void TreeBillBoardRenderer::AddTree(ID3D11Device* device, XMFLOAT3 & pos, XMFLOAT2 & size)
{
	//���� ��Ͽ� �߰�
	treeVertices.push_back({ pos,size });
	//���ο� ����������� ���۸� �����.
	mesh->InitVB(device, treeVertices);
	
	//�ε��� �߰�
	mesh->indices.push_back(treeVertices.size() - 1);
	//���ο� �ε���������� ���۸� �����.
	mesh->InitIB(device);
	//subset�� �ε������� ����
	mesh->subsets[0].IndexCount = mesh->indices.size();
}

void TreeBillBoardRenderer::DeleteTree(ID3D11Device * device, UINT treeIdx)
{
	//���� ����
	treeVertices.erase(treeVertices.begin() + treeIdx);
	mesh->InitVB(device, treeVertices);
	//�ε��� ����
	mesh->indices.erase(mesh->indices.begin() + treeIdx);
	mesh->InitIB(device);
	//subset�� �ε������� ����
	mesh->subsets[0].IndexCount = mesh->indices.size();
}


void TreeBillBoardRenderer::InitDiffuseMaps()
{
	for (auto elem : diffuseMaps)
	{
		if (elem != nullptr)
			ReleaseCOM(elem);
	}
	diffuseMaps.clear();
	
	TextureMgr& texMgr = TextureMgr::Instance();
	ID3D11ShaderResourceView* srv = d3dHelper::CreateTexture2DArraySRV(texMgr.md3dDevice, texMgr.m_context,
		treeTextureNames, DXGI_FORMAT_R8G8B8A8_UNORM);
	diffuseMaps.push_back(srv);
}

void TreeBillBoardRenderer::InitEffects()
{
	effects.clear();
	m_effectMgr.SetType(L"FX/TreeSprite.fxo", EffectType::BillBoardEffectType);
	Effect* effect = m_effectMgr.GetEffect(L"FX/TreeSprite.fxo");
	effects.push_back(effect);
}

void TreeBillBoardRenderer::SetVB(ID3D11DeviceContext * context)
{
	UINT stride[1] = { sizeof(TreePointSprite) };
	UINT offset[1] = { 0 };
	ID3D11Buffer* vb = mesh->GetVB();
	context->IASetVertexBuffers(0, 1, &vb, stride, offset);
}
