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
	//혼합 여부를 나타내는 변수
	bool m_blending; 
	//해당 렌더러가 인스턴싱의 몇번째 인덱스인지 저장해두는 변수
	UINT m_instancingIdx;
	//static 오브젝트 여부를 나타내는 변수
	bool m_static;
public:
	//color
	XMFLOAT4 m_color;
	//해당 렌더러를 가지고 있는 Octree Node와 Object List에서의 iterator
	std::pair<std::list<Renderer*>*, std::list<Renderer*>::iterator*> m_octreeData;
	

#pragma region ShadowVariable
	//그림자를 받을 것인지 나타내는 변수
	bool isShadowed;
	//그림자맵을 렌더링 할 것인지 나타내는 변수
	bool isShadowBaking;

	//현재 쉐도우맵 렌더링인지 나타내는 변수
	bool isRenderShadowMapBaking = { 0 };

	bool isShadowMapRender() { return isShadowBaking && isRenderShadowMapBaking; }
	bool isShadowRender() { return isShadowed && !isRenderShadowMapBaking; }
#pragma endregion


public:
	bool GetBlending() { return m_blending; }
	bool SetBlending(bool blend) { return m_blending = blend;}

	//인스턴싱 관련 함수
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

	// Component을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;

	//렌더링 시 호출되는 함수
	virtual void Draw(ID3D11DeviceContext* context, Camera* camera);

	//버퍼 세팅
	virtual void SetVB(ID3D11DeviceContext* context) { mesh->SetVB(context); }
	
public:
	Renderer(const std::string& id, ComponentType type, const gameObjectID& ownerObj);
	Renderer(const std::string& id, ComponentType type, const gameObjectID& ownerObj, Mesh* mesh);
	//Renderer(std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	virtual ~Renderer();
	Renderer& operator=(const Renderer& other);

public:
	//맵 초기화
	void MapsInit();

	void InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	virtual void InitDiffuseMaps();
	//idx번째 메테리얼의 Map을 수정하는 함수
	void ModifyDiffuseMap(int materialIdx, const std::wstring& mapName, UINT mapType);

	void InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitNormalMaps();
	
	//쉐이더 이펙트 초기화
	void InitEffects(EffectMgr& effectMgr, const std::wstring& shaderPath);
	virtual void InitEffects();
	virtual void InitEffects(const std::vector<std::wstring>& shaderNames, std::vector<EffectType>& effectTypes);

	
	//반드시 SetMesh가 SetMaterial보다 먼저 실행되야 하기 때문에 함수를 합침.
	//기본 메테리얼로 생성
	virtual void SetMesh(Mesh* meshSrc);
	//메테리얼 데이터를 로드해서 생성
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

	// Component을(를) 통해 상속됨
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
	//뼈 1개당 정점 7개(정오면체 두개 붙인 수정모양)
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
	//bonedata를 이미 읽었는지 확인
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

