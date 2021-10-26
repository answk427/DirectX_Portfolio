#include "Renderer.h"
//******************Renderer �Լ� ���� ************************//
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

Renderer::Renderer(const std::string& id) : Component(id)
{
}

Renderer::~Renderer()
{
}

void Renderer::InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath)
{
	for (auto& elem : materials)
	{	
		ID3D11ShaderResourceView* srv = texMgr.CreateTexture(texturePath + elem.diffuseMapName);
		diffuseMaps.push_back(srv);
	}
}

void Renderer::InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath)
{
	for (auto& elem : materials)
	{
		ID3D11ShaderResourceView* srv = texMgr.CreateTexture(texturePath + elem.normalMapName);
		normalMaps.push_back(srv);
	}
}

void Renderer::InitEffects(EffectMgr & effectMgr, const std::wstring& shaderPath)
{
	for (auto& elem : materials)
	{
		//Effect* effect = effectMgr.GetEffect(shaderPath + elem.ShaderName);
		Effect* effect = effectMgr.GetEffect(shaderPath + L"Basic.fxo");
		effects.push_back(effect);
	}	
}

void Renderer::SetMaterials(vector<GeneralMaterial>& materialSrc)
{
	//materials�� �� ���ͷ� ����
	vector<GeneralMaterial>().swap(materials);
	//�Ű����� ���Ϳ� ��ȯ
	materials.swap(materialSrc);
}


//***********************MeshRenderer **************************//

MeshRenderer::MeshRenderer(const std::string& id) : Renderer(id)
{
}

void MeshRenderer::Draw(ID3D11DeviceContext * context, Camera* camera)
{
	XMMATRIX world = XMLoadFloat4x4(&transform->m_world);

	//��������, �ε������۸� �Է������⿡ ����
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

SkinnedMeshRenderer::SkinnedMeshRenderer(const std::string & id) : Renderer(id)
{
}

void SkinnedMeshRenderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
}
