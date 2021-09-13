#pragma once
#include <TextureMgr.h>
#include <D3D11.h>
#include <d3dUtil.h>
#include <vector>

#include "Interface.h"
#include "Mesh.h"
#include "Material.h"
#include "EffectMgr.h"





class Renderer : public Component
{
protected:
	Mesh* mesh;
	vector<GeneralMaterial> materials;
	vector<ID3D11ShaderResourceView*> diffuseMaps;
	vector<ID3D11ShaderResourceView*> normalMaps;
	vector<Effect*> effects;
public:
	// Component을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;
	virtual void Draw(ID3D11DeviceContext* context, Camera* camera) = 0;

	Renderer();
	//Renderer(std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	
	~Renderer();

	void InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitEffects(EffectMgr& effectMgr, const std::wstring& shaderPath);
	void SetMesh(Mesh* meshSrc) { mesh = meshSrc; }
	void SetMaterials(vector<GeneralMaterial>& materialSrc);
};


class MeshRenderer : public Renderer
{
public:
	MeshRenderer();
	// Renderer을(를) 통해 상속됨
	virtual void Draw(ID3D11DeviceContext * context, Camera* camera) override;
};

class SkinnedMeshRenderer : public Renderer
{

};