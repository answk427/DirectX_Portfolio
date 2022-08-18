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
#include "NodeBoneDatas.h"
#include "AssimpLoader.h"
#include "SkinningComputeShader.h"

#include <SkinnedData.h>


class TesselationData
{
public:
	TesselationData() : minDist(20.0f), maxDist(500.0f), minTess(0.0f), maxTess(6.0f) {}
public:
	float minDist;
	float maxDist;
	float minTess;
	float maxTess;
public:
	float GetMinDist() { return minDist; }
	float GetMaxDist() { return maxDist; }
	float GetMinTess() { return minTess; }
	float GetMaxTess() { return maxTess; }
	void SetMinDist(float minDist) { this->minDist = minDist; }
	void SetMaxDist(float maxDist) { this->maxDist = maxDist; }
	void SetMinTess(float minTess) { this->minTess = minTess; }
	void SetMaxTess(float maxTess) { this->maxTess = maxTess; }
};



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
	std::weak_ptr<NodeHierarchy> m_bones;
	
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

	//���� ������� ���������� ��Ÿ���� ����
	bool isRenderShadowMapBaking = { 0 };

	bool isShadowMapRender() { return isShadowBaking && isRenderShadowMapBaking; }
	bool isShadowRender() { return isShadowed && !isRenderShadowMapBaking; }
#pragma endregion


public:
	bool GetBlending() { return m_blending; }
	bool SetBlending(bool blend) { return m_blending = blend;}

	//�ν��Ͻ� ���� �Լ�
	virtual bool GetInstancing()
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
	virtual void InstancingUpdate();

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
	Renderer(const std::string& id, ComponentType type, const gameObjectID& ownerObj);
	Renderer(const std::string& id, ComponentType type, const gameObjectID& ownerObj, Mesh* mesh);
	//Renderer(std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	virtual ~Renderer();
	Renderer& operator=(const Renderer& other);

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
	virtual void SetMesh(Mesh* meshSrc);
	//���׸��� �����͸� �ε��ؼ� ����
	virtual void SetMesh(Mesh* meshSrc, std::vector<GeneralMaterial>& materialSrc);
	Mesh* GetMesh() { return mesh; }
	const Mesh* GetMesh() const { return mesh; }
	
public:
	std::vector<GeneralMaterial>& GetMaterials() { return materials; }
	
	Transform* GetTransform() { return transform; }
	const Transform* GetTransform() const { return transform; }
	virtual void GetWorldMatrix(XMMATRIX& dest) { m_bones.lock()->GetFinalTransform(dest, ownerObjectId); }
	virtual void GetWorldMatrix(XMFLOAT4X4& dest)
	{ 
		XMMATRIX tempDest;
		m_bones.lock()->GetFinalTransform(tempDest, ownerObjectId); 
		XMStoreFloat4x4(&dest, tempDest);
	}
	void SetTransform(Transform* tr) { transform = tr; }
	
	virtual void SetNodeHierarchy(std::weak_ptr<NodeHierarchy> bones) { m_bones = bones; }
	

	virtual void SetTechniqueType(int orTechnique) { m_technique_type = orTechnique; }
	virtual UINT GetTechniqueType() { return m_technique_type; }

	void SetStaticObject(bool setting) { m_static = setting; }
	bool GetStaticObject() {return m_static;}

	// Component��(��) ���� ��ӵ�
	virtual void Enable() override;
	virtual void Disable() override;

	
protected:
	void SetMaterials(std::vector<GeneralMaterial>& materialSrc);
};

class BoneRenderer : public Renderer
{
public:
#define CNTPERVERTEX 7
#define CRYSTALSIZE 2

	enum PositionNum
	{
		FLOORRIGHT = 0,
		FLOORBACK = 1,
		FLOORLEFT=2,
		FLOORFRONT=3,
		UPY=4,
		DOWNY=5,
		POS=6
	};
public:
	BoneRenderer(const std::string &id, const gameObjectID &ownerObj,
		std::weak_ptr<NodeHierarchy> nodes);
	
	~BoneRenderer()
	{
		if (mesh != nullptr)
			delete mesh;
	}
private:
	//�� 1���� ���� 7��(������ü �ΰ� ���� �������)
	std::vector<Vertex::Basic32> vertices;
	//std::weak_ptr<Animator> m_Animator;
	

	void CreateBoneShape(std::vector<Vertex::Basic32>& result, XMFLOAT3& pos, XMFLOAT3& xAxis,
		XMFLOAT3& yAxis, XMFLOAT3& zAxis);
	void InitIndices(std::vector<UINT>& indices);
	void InitVertices();
	void UpdateVB();
public:
	virtual void Update() override;
	virtual void SetVB(ID3D11DeviceContext* context);
	void SetMesh();
	


};


class MeshRenderer : public Renderer
{
public:
	MeshRenderer(const std::string& id, const gameObjectID& ownerId);
	MeshRenderer& operator=(const MeshRenderer& other);
};

class SkinnedMeshRenderer : public Renderer
{
private:
	GameTimer& m_timer;
	BoneDatas tempBoneDatas;
	//bonedata�� �̹� �о����� Ȯ��
	bool readBoneData;
	UINT shadowSkinningInstancingSize = 0;
public:
	bool boneDrawMode = false;
public:
	SkinnedMeshRenderer(const std::string& id, const gameObjectID& ownerId);
	SkinnedMeshRenderer(const SkinnedMeshRenderer& other);
	//SkinnedMeshRenderer(SkinnedMeshRenderer&& other);
	SkinnedMeshRenderer& operator=(const SkinnedMeshRenderer& skinRenderer);
public:
	//std::shared_ptr<Animator> m_animator;
	std::unique_ptr<BoneRenderer> mBoneRenderer;
	std::vector<MyVertex::SkinnedData> m_skinnedDatas;
	std::unique_ptr<SkinningComputeShader> mSkinningComputeShader;
	
	
public:
	virtual void Draw(ID3D11DeviceContext* context, Camera* camera);
	virtual void GetWorldMatrix(XMMATRIX& dest) { m_bones.lock()->GetRootWorldTransform(dest); }
	virtual void GetWorldMatrix(XMFLOAT4X4& dest) { m_bones.lock()->GetRootWorldTransform(dest); }
	virtual void Update() override;
	virtual void InstancingUpdate();
	
public:
	void SetAnimationClip(const std::string& clipName);
	void LoadAnimationClip(MyAnimationClip& clip);
	void DeleteAnimationClip(const std::string& clipName);
	void InitSkinnedVB();
	void ToggleDrawMode() { boneDrawMode = !boneDrawMode; }
	std::vector<std::string> GetAnimationClipNames();
	
	void SetBoneDatas(BoneDatas& boneDatas);
	
	virtual void SetNodeHierarchy(std::weak_ptr<NodeHierarchy> bones);

	virtual void SetTechniqueType(int orTechnique);
	virtual UINT GetTechniqueType();
	virtual bool GetInstancing()
	{
		if (mesh != nullptr)
			return mesh->GetInstancing();
		return	false;
	}
	
	virtual void SetMesh(Mesh* meshSrc);
	virtual void SetMesh(Mesh* meshSrc, std::vector<GeneralMaterial>& materialSrc);

};

