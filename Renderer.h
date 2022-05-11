#pragma once
#include <TextureMgr.h>
#include <D3D11.h>
#include <d3dUtil.h>

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
	std::vector<GeneralMaterial> materials;
	std::vector<ID3D11ShaderResourceView*> diffuseMaps;
	std::vector<ID3D11ShaderResourceView*> normalMaps;
	std::vector<Effect*> effects;
	TextureMgr& m_texMgr;
	EffectMgr& m_effectMgr;
	
protected:
	UINT m_technique_type;
	//ȥ�� ���θ� ��Ÿ���� ����
	bool m_blending; 
	//�ش� �������� �ν��Ͻ��� ���° �ε������� �����صδ� ����
	UINT m_instancingIdx;
	//static ������Ʈ ���θ� ��Ÿ���� ����
	bool m_static;
public:
	//color
	XMFLOAT4 m_color;
	//�ش� �������� ������ �ִ� Octree Node�� Object List������ iterator
	std::pair<std::list<Renderer*>*, std::list<Renderer*>::iterator*> m_octreeData;
	

#pragma region ShadowVariable
	//�׸��ڸ� ���� ������ ��Ÿ���� ����
	bool isShadowed;
	//�׸��ڸ��� ������ �� ������ ��Ÿ���� ����
	bool isShadowBaking;

	//���� ��������� ������ �ƴ��� �ȵƴ��� ��Ÿ���� ����
	bool isRenderShadowMapBaking = { 0 };
#pragma endregion


public:
	bool GetBlending() { return m_blending; }
	bool SetBlending(bool blend) { return m_blending = blend;}

	//�ν��Ͻ� ���� �Լ�
	bool GetInstancing()
	{ 
		if (mesh != nullptr)
			return mesh->GetInstancing() && !isRenderShadowMapBaking;
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

	//���� ����
	virtual void SetVB(ID3D11DeviceContext* context) { mesh->SetVB(context); }
	
public:
	Renderer(const std::string& id, ComponentType type);
	Renderer(const std::string& id, ComponentType type, Mesh* mesh);
	//Renderer(std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	~Renderer();

public:
	//�� �ʱ�ȭ
	void MapsInit();

	void InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	virtual void InitDiffuseMaps();
	//idx��° ���׸����� Map�� �����ϴ� �Լ�
	void ModifyDiffuseMap(int materialIdx, const std::wstring& mapName, UINT mapType);

	void InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitNormalMaps();
	
	//���̴� ����Ʈ �ʱ�ȭ
	void InitEffects(EffectMgr& effectMgr, const std::wstring& shaderPath);
	virtual void InitEffects();
	virtual void InitEffects(const std::vector<std::wstring>& shaderNames, std::vector<EffectType>& effectTypes);

	
	//�ݵ�� SetMesh�� SetMaterial���� ���� ����Ǿ� �ϱ� ������ �Լ��� ��ħ.
	//�⺻ ���׸���� ����
	void SetMesh(Mesh* meshSrc);
	//���׸��� �����͸� �ε��ؼ� ����
	void SetMesh(Mesh* meshSrc, std::vector<GeneralMaterial>& materialSrc);
	Mesh* GetMesh() { return mesh; }
	const Mesh* GetMesh() const { return mesh; }
	
public:
	std::vector<GeneralMaterial>& GetMaterials() { return materials; }

	void SetTransform(Transform* tr) { transform = tr; }
	Transform* GetTransform() { return transform; }
	const Transform* GetTransform() const { return transform; }

	void SetTechniqueType(int orTechnique) { m_technique_type = orTechnique; }

	void SetStaticObject(bool setting) { m_static = setting; }
	bool GetStaticObject() {return m_static;}

	// Component��(��) ���� ��ӵ�
	virtual void Enable() override;
	virtual void Disable() override;
protected:
	void SetMaterials(std::vector<GeneralMaterial>& materialSrc);
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