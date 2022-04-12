#include "Renderer.h"

//******************Renderer 함수 정의 ************************//
void Renderer::Init()
{
	
}


void Renderer::Update()
{
	
}

void Renderer::FixedUpdate()
{
}

void Renderer::LateUpdate()
{
}

void Renderer::AddInstancingQueue()
{
	mesh->InstancingDatas.push_back(new InstancingData());
	m_instancingIdx = mesh->InstancingDatas.size() - 1;
	
	for (int i = 0; i < materials.size(); ++i)
	{
		//i번째 subset에 해당하는 diffuseMap 이름을 설정
		mesh->textureNames[i].push_back(materials[i].diffuseMapName);
	}
	//texture배열의 사이즈를 변경하거나 새로생성
	TextureMgr& textureMgr = TextureMgr::Instance();
	mesh->CreateTextureArrayResourceView(textureMgr.md3dDevice, textureMgr.m_context);
}

void Renderer::InstancingUpdate()
{
	//instancing 중이 아니면 return
	if (!GetInstancing() || mesh == nullptr)
		return;
	
	//세계행렬 업데이트
	mesh->InstancingDatas[m_instancingIdx]->world = transform->m_world;
	//역전치행렬 업데이트
	XMStoreFloat4x4(&mesh->InstancingDatas[m_instancingIdx]->worldInvTranspose,
		MathHelper::InverseTranspose(
			XMLoadFloat4x4(&mesh->InstancingDatas[m_instancingIdx]->world)));
	//rgb 업데이트
	mesh->InstancingDatas[m_instancingIdx]->color = m_color;
	//현재 렌더러를 알려주는 인덱스
	mesh->InstancingDatas[m_instancingIdx]->RendererIdx = m_instancingIdx;

	//이번 프레임에 렌더링할 오브젝트로 등록
	mesh->enableInstancingIndexes.push_back(m_instancingIdx);
}

void Renderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (mesh == nullptr)
		return;

	XMMATRIX world = XMLoadFloat4x4(&transform->m_world);
		
	
	//정점버퍼, 인덱스버퍼를 입력조립기에 묶음
	if (!GetInstancing())
		SetVB(context);
	else
	{
		//이미 렌더링을 했거나, 그릴 오브젝트들이 없는 경우 리턴
		if (mesh->enableInstancingIndexes.empty())
			return;
		mesh->InstancingUpdate(context);
		mesh->SetInstanceVB(context);
	}
		

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
				if(!effects[i]->IASetting(context, m_technique_type))
					break;
			}
			//출력병합기 세팅
			if (!effects[i]->OMSetting(context, m_blending))
				break;
		}
		
		if(GetInstancing())
			activeTech = effects[i]->GetTechnique(m_technique_type | TechniqueType::Instancing);
		else
			activeTech = effects[i]->GetTechnique(m_technique_type);

		activeTech->GetDesc(&techDesc);
		


		//shader에 필요한 데이터 설정
		effects[i]->PerObjectSet(&materials[i],
			camera, world);

		//인스턴싱일 때 텍스쳐배열 사용
		if(GetInstancing())
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
}

Renderer::Renderer(const std::string& id, ComponentType type) : Component(id,type)
, m_texMgr(TextureMgr::Instance()), m_effectMgr(EffectMgr::Instance()), m_color(1.0f,1.0f,1.0f,1.0f),
mesh(0), m_blending(0), m_technique_type(TechniqueType::Light), m_instancingIdx(0), m_octreeData({ 0,0 })
{
	MapsInit();
	SetMaterials(std::vector<GeneralMaterial>(1));
	InitEffects();
}

Renderer::Renderer(const std::string & id, ComponentType type, Mesh * mesh) : Component(id, type)
, m_texMgr(TextureMgr::Instance()), m_effectMgr(EffectMgr::Instance()), m_color(1.0f, 1.0f, 1.0f, 1.0f),
mesh(0), m_blending(0), m_technique_type(TechniqueType::Light), m_instancingIdx(0), m_octreeData({ 0,0 })
{
	SetMesh(mesh);
	SetMaterials(std::vector<GeneralMaterial>(1));
	MapsInit();
	InitEffects();
}

Renderer::~Renderer()
{

}

void Renderer::InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath)
{
	diffuseMaps.clear();
	for (auto& elem : materials)
	{	
		ID3D11ShaderResourceView* srv = texMgr.CreateTexture(texturePath + elem.diffuseMapName);
		diffuseMaps.push_back(srv);
	}
}

void Renderer::InitDiffuseMaps()
{
	diffuseMaps.clear();
	for (auto& elem : materials)
	{
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(elem.diffuseMapName);
		diffuseMaps.push_back(srv);
	}
}

void Renderer::ModifyDiffuseMap(int materialIdx, const std::wstring & mapName, UINT mapType)
{
	TextureMgr& textureMgr = TextureMgr::Instance();
	if (mapType == 0)
	{
		materials[materialIdx].diffuseMapName = mapName;
		
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(mapName);
		diffuseMaps[materialIdx] = srv;

		mesh->ModifyTextureArraySubResource(textureMgr.md3dDevice, textureMgr.m_context,
			m_instancingIdx, materialIdx, mapName);
	}
	else if (mapType == 1)
	{
		materials[materialIdx].normalMapName = mapName;
		
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(mapName);
		normalMaps[materialIdx] = srv;
	}
		
}

void Renderer::InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath)
{
	normalMaps.clear();
	for (auto& elem : materials)
	{
		ID3D11ShaderResourceView* srv = texMgr.CreateTexture(texturePath + elem.normalMapName);
		normalMaps.push_back(srv);
	}
}

void Renderer::InitNormalMaps()
{
	normalMaps.clear();
	for (auto& elem : materials)
	{
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(elem.normalMapName);
		normalMaps.push_back(srv);
	}
}

void Renderer::InitEffects(EffectMgr & effectMgr, const std::wstring& shaderPath)
{
	effects.clear();
	for (auto& elem : materials)
	{
		//Effect* effect = effectMgr.GetEffect(shaderPath + elem.ShaderName);
		Effect* effect = effectMgr.GetEffect(shaderPath + L"Basic.fxo");
		effects.push_back(effect);
	}		
}

void Renderer::InitEffects()
{
	effects.clear();
	for (auto& elem : materials)
	{
		//Effect* effect = effectMgr.GetEffect(shaderPath + elem.ShaderName);
		//Effect* effect = m_effectMgr.GetEffect(elem.ShaderName);
		Effect* effect = m_effectMgr.GetEffect(L"FX/Basic.fxo");
		effects.push_back(effect);
	}
}

void Renderer::InitEffects(const std::vector<std::wstring>& shaderNames)
{
	effects.clear();
	for (auto& shaderName : shaderNames)
	{
		Effect* effect = m_effectMgr.GetEffect(shaderName);
		assert(!effect);
		effects.push_back(effect);
	}
}

void Renderer::SetMesh(Mesh * meshSrc)
{
	//원래 설정되어 있던 mesh의 InstancingData에서 현재 오브젝트의 데이터를 지움.
	if (mesh != nullptr)
	{
		if (mesh->InstancingDatas[m_instancingIdx] != nullptr)
			delete mesh->InstancingDatas[m_instancingIdx];
		mesh->InstancingDatas[m_instancingIdx] = nullptr;
	}

	mesh = meshSrc;

	//Material을 Subset 개수만큼 기본으로 설정
	materials.clear();
	materials.assign(mesh->GetSubsetLength(), GeneralMaterial());

	AddInstancingQueue();
}

void Renderer::SetMesh(Mesh * meshSrc, vector<GeneralMaterial>& materialSrc)
{
	//원래 설정되어 있던 mesh의 InstancingData에서 현재 오브젝트의 데이터를 지움.
	if (mesh != nullptr)
	{
		if (mesh->InstancingDatas[m_instancingIdx] != nullptr)
			delete mesh->InstancingDatas[m_instancingIdx];
		mesh->InstancingDatas[m_instancingIdx] = nullptr;
	}

	mesh = meshSrc;

	SetMaterials(materialSrc);
	AddInstancingQueue();
}

void Renderer::MapsInit()
{
	InitDiffuseMaps();
	InitNormalMaps();
}

void Renderer::SetMaterials(vector<GeneralMaterial>& materialSrc)
{
	//materials를 빈 벡터로 만듬
	vector<GeneralMaterial>().swap(materials);
	//매개변수 벡터와 교환
	materials.swap(materialSrc);
	MapsInit();
}

void Renderer::Enable()
{
}

void Renderer::Disable()
{
}


//***********************MeshRenderer **************************//

MeshRenderer::MeshRenderer(const std::string& id) : Renderer(id, ComponentType::MESHRENDERER)
{
	SetTechniqueType(TechniqueType::Light | TechniqueType::DiffuseMap);
}


MeshRenderer & MeshRenderer::operator=(const MeshRenderer & meshrenderer)
{
	transform = meshrenderer.transform;
	mesh = meshrenderer.mesh;
	materials = meshrenderer.materials;
	diffuseMaps = meshrenderer.diffuseMaps;
	normalMaps = meshrenderer.normalMaps;
	effects = meshrenderer.effects;
	m_texMgr = meshrenderer.m_texMgr;
	m_effectMgr = meshrenderer.m_effectMgr;

	return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
}



SkinnedMeshRenderer::SkinnedMeshRenderer(const std::string & id) : Renderer(id, ComponentType::SKINNEDMESHRENDERER)
{
}

SkinnedMeshRenderer & SkinnedMeshRenderer::operator=(const SkinnedMeshRenderer & skinRenderer)
{
	transform = skinRenderer.transform;
	mesh = skinRenderer.mesh;
	materials = skinRenderer.materials;
	diffuseMaps = skinRenderer.diffuseMaps;
	normalMaps = skinRenderer.normalMaps;
	effects = skinRenderer.effects;
	m_texMgr = skinRenderer.m_texMgr;
	m_effectMgr = skinRenderer.m_effectMgr;

	return *this;
	// TODO: 여기에 반환 구문을 삽입합니다.
}

