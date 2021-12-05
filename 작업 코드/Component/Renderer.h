#pragma once
#include <TextureMgr.h>
#include <D3D11.h>
#include <d3dUtil.h>
#include <vector>

#include "Interface.h"
#include "Mesh.h"
#include "Material.h"
#include "EffectMgr.h"
#include "Transform.h"





class Renderer : public Component
{
protected:
	Transform* transform;
	Mesh* mesh;
	vector<GeneralMaterial> materials;
	vector<ID3D11ShaderResourceView*> diffuseMaps;
	vector<ID3D11ShaderResourceView*> normalMaps;
	vector<Effect*> effects;
	TextureMgr& m_texMgr;
	EffectMgr& m_effectMgr;

public:
	// Component을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;
	virtual void Draw(ID3D11DeviceContext* context, Camera* camera) = 0;

public:
	Renderer(const std::string& id, ComponentType type);
	//Renderer(std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	~Renderer();

public:
	void InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitDiffuseMaps();

	void InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitNormalMaps();

	void InitEffects(EffectMgr& effectMgr, const std::wstring& shaderPath);
	void InitEffects();

	void MapsInit();
	
	void SetMesh(Mesh* meshSrc) { mesh = meshSrc; }
	const Mesh* GetMesh() { return mesh; }
	
	void SetMaterials(vector<GeneralMaterial>& materialSrc);
	vector<GeneralMaterial>& GetMaterials() { return materials; }

	void SetTransform(Transform* tr) { transform = tr; }
};


class MeshRenderer : public Renderer
{
public:
	MeshRenderer(const std::string& id);
	// Renderer을(를) 통해 상속됨
	virtual void Draw(ID3D11DeviceContext * context, Camera* camera) override;
	MeshRenderer& operator=(const MeshRenderer& meshrenderer);
};

class SkinnedMeshRenderer : public Renderer
{
public:
	SkinnedMeshRenderer(const std::string& id);
	SkinnedMeshRenderer& operator=(const SkinnedMeshRenderer& skinRenderer);
	// Renderer을(를) 통해 상속됨
	virtual void Draw(ID3D11DeviceContext * context, Camera * camera) override;
};