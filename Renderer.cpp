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

void Renderer::SetMaterials(vector<GeneralMaterial>& materialSrc)
{
	//materials�� �� ���ͷ� ����
	vector<GeneralMaterial>().swap(materials);
	//�Ű����� ���Ϳ� ��ȯ
	materials.swap(materialSrc);
}


//***********************MeshRenderer �Լ� ���� **************************//

MeshRenderer::MeshRenderer()
{
}

void MeshRenderer::Draw(ID3D11DeviceContext * context)
{
	//��������, �ε������۸� �Է������⿡ ����
	mesh->SetVB(context);
	mesh->SetIB(context);
	
	int subsetLength = mesh->GetSubsetLength();
	for(int i=0; i<subsetLength; i++)
		mesh->Draw(context,i);
}
