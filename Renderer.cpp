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

Renderer::Renderer()
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
		Effect* effect = effectMgr.GetEffect(shaderPath + elem.ShaderName);
		effects.push_back(effect);
	}	
}

void Renderer::SetMaterials(vector<GeneralMaterial>& materialSrc)
{
	//materials를 빈 벡터로 만듬
	vector<GeneralMaterial>().swap(materials);
	//매개변수 벡터와 교환
	materials.swap(materialSrc);
}


//***********************MeshRenderer 함수 정의 **************************//

MeshRenderer::MeshRenderer()
{
}

void MeshRenderer::Draw(ID3D11DeviceContext * context, Camera* camera)
{
	//정점버퍼, 인덱스버퍼를 입력조립기에 묶음
	mesh->SetVB(context);
	mesh->SetIB(context);
	
	int subsetLength = mesh->GetSubsetLength();
	for (int i = 0; i < subsetLength; i++)
	{
		//shader에 필요한 데이터 설정
		effects[i]->PerObjectSet(&materials[i],
			camera, &mesh->InstancingDatas[i]);
		

		mesh->Draw(context, i);
	}
}
