#include "TreeBillBoardRenderer.h"

TreeBillBoardRenderer::TreeBillBoardRenderer() : Renderer("TreeBillBoardRenderer", ComponentType::MESHRENDERER)
{
	TextureMgr& texMgr = TextureMgr::Instance();

	//BillBoard 메쉬 생성
	mesh = new Mesh("TreeBillBoardMesh");
	mesh->Init(std::vector<MyVertex::BasicVertex>(),
		std::vector<UINT>());
	mesh->InitVB<TreePointSprite>(texMgr.md3dDevice, treeVertices);
	mesh->InitIB(texMgr.md3dDevice);
			
	//texture 추가
	treeTextureNames.push_back(L"Textures/tree0.dds");
	treeTextureNames.push_back(L"Textures/tree1.dds");
	treeTextureNames.push_back(L"Textures/tree2.dds");
	treeTextureNames.push_back(L"Textures/tree3.dds");
	
	//subset1개, 기본 material 1개 사용
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
	//정점 목록에 추가
	treeVertices.push_back({ pos,size });
	//새로운 정점목록으로 버퍼를 만든다.
	mesh->InitVB(device, treeVertices);
	
	//인덱스 추가
	mesh->indices.push_back(treeVertices.size() - 1);
	//새로운 인덱스목록으로 버퍼를 만든다.
	mesh->InitIB(device);
	//subset의 인덱스개수 변경
	mesh->subsets[0].IndexCount = mesh->indices.size();
}

void TreeBillBoardRenderer::DeleteTree(ID3D11Device * device, UINT treeIdx)
{
	//정점 제거
	treeVertices.erase(treeVertices.begin() + treeIdx);
	mesh->InitVB(device, treeVertices);
	//인덱스 제거
	mesh->indices.erase(mesh->indices.begin() + treeIdx);
	mesh->InitIB(device);
	//subset의 인덱스개수 변경
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
