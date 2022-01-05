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

Renderer::Renderer(const std::string& id, ComponentType type) : Component(id,type)
, m_texMgr(TextureMgr::Instance()), m_effectMgr(EffectMgr::Instance()), mesh(0)
{
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
		Effect* effect = m_effectMgr.GetEffect(elem.ShaderName);
		effects.push_back(effect);
	}
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
}

void MeshRenderer::Draw(ID3D11DeviceContext * context, Camera* camera)
{
	if (mesh == nullptr)
		return;

	MapsInit();
	XMMATRIX world = XMLoadFloat4x4(&transform->m_world);

	//정점버퍼, 인덱스버퍼를 입력조립기에 묶음
	mesh->SetVB(context);
	mesh->SetIB(context);
	
	int subsetLength = mesh->GetSubsetLength();
	
	
	ID3DX11EffectTechnique* activeTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	
	for (int i = 0; i < subsetLength; i++)
	{
		activeTech = effects[i]->GetTechnique(TechniqueType::Light |
			TechniqueType::DiffuseMap);
		activeTech->GetDesc(&techDesc);

			
		//shader에 필요한 데이터 설정
		effects[i]->PerObjectSet(&materials[i],
			camera, world);
		
		effects[i]->SetMaps(diffuseMaps[i], normalMaps[i], nullptr);

						
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			activeTech->GetPassByIndex(p)->Apply(0, context);
			mesh->Draw(context, i);
		}
	}
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

void SkinnedMeshRenderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
}
