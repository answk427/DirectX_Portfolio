#include "TreeBillBoardRenderer.h"

TreeBillBoardRenderer::TreeBillBoardRenderer() : Renderer("TreeBillBoardRenderer", ComponentType::MESHRENDERER)
{
	TextureMgr& texMgr = TextureMgr::Instance();
	//그림자맵 렌더링에 사용할 쉐이더
	shadowEffect = std::make_unique<BuildShadowMapBilboardEffect>
		(texMgr.md3dDevice, L"FX/BuildShadowMapBilboard.fxo");

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

void TreeBillBoardRenderer::InitEffects(const std::vector<std::wstring>& shaderNames, std::vector<EffectType>& effectTypes)
{
	effects.clear();

	UINT subsetSize = mesh->GetSubsetLength();

	for (int i = 0; i < shaderNames.size(); ++i)
	{
		m_effectMgr.SetType(shaderNames[i], effectTypes[i]);
		Effect* effect = shadowEffect.get();
		assert(effect != nullptr);
		effects.push_back(effect);
	}

	for (int i = shaderNames.size(); i < subsetSize; ++i)
	{
		Effect* effect = shadowEffect.get();
		assert(effect != nullptr);
		effects.push_back(effect);
	}
}

void TreeBillBoardRenderer::SetVB(ID3D11DeviceContext * context)
{
	UINT stride[1] = { sizeof(TreePointSprite) };
	UINT offset[1] = { 0 };
	ID3D11Buffer* vb = mesh->GetVB();
	context->IASetVertexBuffers(0, 1, &vb, stride, offset);
}

void TreeBillBoardRenderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (mesh == nullptr)
		return;

	XMMATRIX world = XMLoadFloat4x4(&transform->m_world);


	//정점버퍼, 인덱스버퍼를 입력조립기에 묶음
	if (GetInstancing())
	{
		//이미 렌더링을 했거나, 그릴 오브젝트들이 없는 경우 리턴
		if (mesh->enableInstancingIndexes.empty())
			return;
		mesh->InstancingUpdate(context);
		mesh->SetInstanceVB(context);
	}
	else
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
			if (GetInstancing())
			{
				if (!effects[i]->IASetting(context, m_technique_type | TechniqueType::Instancing))
					break;
			}
			else
			{
				if (!effects[i]->IASetting(context, m_technique_type))
					break;
			}
			//출력병합기 세팅
			if (!effects[i]->OMSetting(context, m_blending))
				break;
		}

		UINT tempTechType = m_technique_type;
		if (isShadowed && isRenderShadowMapBaking)
			tempTechType = tempTechType | TechniqueType::Shadowed;


		if (GetInstancing())
			activeTech = effects[i]->GetTechnique(tempTechType | TechniqueType::Instancing);
		else
			activeTech = effects[i]->GetTechnique(tempTechType);

		activeTech->GetDesc(&techDesc);



		//shader에 필요한 데이터 설정
		effects[i]->PerObjectSet(&materials[i],
			camera, world);

		//인스턴싱일 때 텍스쳐배열 사용
		if (GetInstancing())
			effects[i]->SetMapArray(mesh->textureArrays[i]);
		else
			effects[i]->SetMaps(diffuseMaps[i], normalMaps[i], nullptr);



		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			activeTech->GetPassByIndex(p)->Apply(0, context);

			mesh->Draw(context, i);
		}
	}
	//instancing 렌더링은 Draw 호출 한번만 하도록 벡터를 비워줌.
	if (GetInstancing())
		mesh->enableInstancingIndexes.clear();

	//현재 렌더링이 그림자맵 렌더링이면
	if (isShadowBaking && isRenderShadowMapBaking)
		isRenderShadowMapBaking = false;
}
