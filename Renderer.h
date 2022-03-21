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
#include "Vertex.h"
#include <MathHelper.h>





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
	

protected:
	UINT m_technique_type;
	//ȥ�� ���θ� ��Ÿ���� ����
	bool m_blending; 
	//�ش� �������� �ν��Ͻ��� ���° �ε������� �����صδ� ����
	UINT m_instancingIdx;
public:
	//color
	XMFLOAT4 m_color;
	
public:
	bool GetBlending() { return m_blending; }
	bool SetBlending(bool blend) { return m_blending = blend;}

	//�ν��Ͻ� ���� �Լ�
	bool GetInstancing()
	{ 
		if (mesh != nullptr)
			return mesh->GetInstancing();
		return	false;
	}
	bool SetInstancing(bool instancing) 
	{ 
		if (mesh != nullptr)
		{
			return mesh->SetInstancing(instancing);
		}
			
		return false;
	}
	
	void AddInstancingQueue();
	void InstancingUpdate();

	// Component��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;

	//������ �� ȣ��Ǵ� �Լ�
	virtual void Draw(ID3D11DeviceContext* context, Camera* camera);
	
public:
	Renderer(const std::string& id, ComponentType type);
	//Renderer(std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	~Renderer();

public:
	//�� �ʱ�ȭ
	void MapsInit();

	void InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitDiffuseMaps();

	void InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitNormalMaps();
	
	//���̴� ����Ʈ �ʱ�ȭ
	void InitEffects(EffectMgr& effectMgr, const std::wstring& shaderPath);
	void InitEffects();

	
	void SetMesh(Mesh* meshSrc)
	{
		//���� �����Ǿ� �ִ� mesh�� InstancingData���� ���� ������Ʈ�� �����͸� ����.
		if (mesh != nullptr)
			mesh->InstancingDatas.erase(mesh->InstancingDatas.begin()+ m_instancingIdx);
		mesh = meshSrc; 
		AddInstancingQueue();
	}

	Mesh* GetMesh() { return mesh; }
	
	void SetMaterials(vector<GeneralMaterial>& materialSrc);
	vector<GeneralMaterial>& GetMaterials() { return materials; }

	void SetTransform(Transform* tr) { transform = tr; }
	Transform* GetTransform() { return transform; }

	void SetTechniqueType(int orTechnique) { m_technique_type = orTechnique; }

	// Component��(��) ���� ��ӵ�
	virtual void Enable() override;
	virtual void Disable() override;
};


class MeshRenderer : public Renderer
{
public:
	MeshRenderer(const std::string& id);
	MeshRenderer& operator=(const MeshRenderer& meshrenderer);
	 
};

class SkinnedMeshRenderer : public Renderer
{
public:
	SkinnedMeshRenderer(const std::string& id);
	SkinnedMeshRenderer& operator=(const SkinnedMeshRenderer& skinRenderer);
	
};