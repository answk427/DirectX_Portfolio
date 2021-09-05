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

Renderer::Renderer(std::wstring& fileName, std::wstring& texturePath,ID3D11Device * device, TextureMgr& texMgr)
{
	InitDiffuseMaps(texMgr, texturePath);
	InitNormalMaps(texMgr, texturePath);
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


//***********************MeshRenderer 함수 정의 **************************//

void MeshRenderer::Draw(ID3D11DeviceContext * context)
{
	for(int i=0; i<mesh->GetSubsetLength(); i++)
		mesh->Draw(context,i);
}
