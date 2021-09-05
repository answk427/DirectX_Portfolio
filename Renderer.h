#pragma once
#include "Interface.h"
#include "Mesh.h"
#include "Material.h"

#include <TextureMgr.h>
#include <D3D11.h>
#include <d3dUtil.h>
#include <vector>


class Renderer : public Component
{
protected:
	Mesh* mesh;
	vector<GeneralMaterial> materials;
	vector<ID3D11ShaderResourceView*> diffuseMaps;
	vector<ID3D11ShaderResourceView*> normalMaps;
public:
	// Component을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;
	virtual void Draw(ID3D11DeviceContext* context) = 0;

	Renderer(std::wstring& fileName, std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	~Renderer();
	void InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath);
};

class MeshRenderer : public Renderer
{
	// Renderer을(를) 통해 상속됨
	virtual void Draw(ID3D11DeviceContext * context) override;
};

class SkinnedMeshRenderer : public Renderer
{

};