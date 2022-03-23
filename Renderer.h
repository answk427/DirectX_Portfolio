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
	//혼합 여부를 나타내는 변수
	bool m_blending; 
	//해당 렌더러가 인스턴싱의 몇번째 인덱스인지 저장해두는 변수
	UINT m_instancingIdx;
public:
	//color
	XMFLOAT4 m_color;
	
public:
	bool GetBlending() { return m_blending; }
	bool SetBlending(bool blend) { return m_blending = blend;}

	//인스턴싱 관련 함수
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

	// Component을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;

	//렌더링 시 호출되는 함수
	virtual void Draw(ID3D11DeviceContext* context, Camera* camera);
	
public:
	Renderer(const std::string& id, ComponentType type);
	//Renderer(std::wstring& texturePath,ID3D11Device* device, TextureMgr& texMgr);
	~Renderer();

public:
	//맵 초기화
	void MapsInit();

	void InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitDiffuseMaps();
	//idx번째 메테리얼의 Map을 수정하는 함수
	void ModifyDiffuseMap(int materialIdx, const std::wstring& mapName, UINT mapType);

	void InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath);
	void InitNormalMaps();
	
	//쉐이더 이펙트 초기화
	void InitEffects(EffectMgr& effectMgr, const std::wstring& shaderPath);
	void InitEffects();

	
	//반드시 SetMesh가 SetMaterial보다 먼저 실행되야 하기 때문에 함수를 합침.
	//기본 메테리얼로 생성
	void SetMesh(Mesh* meshSrc);
	//메테리얼 데이터를 로드해서 생성
	void SetMesh(Mesh* meshSrc, vector<GeneralMaterial>& materialSrc);
	Mesh* GetMesh() { return mesh; }
	
	
	vector<GeneralMaterial>& GetMaterials() { return materials; }

	void SetTransform(Transform* tr) { transform = tr; }
	Transform* GetTransform() { return transform; }

	void SetTechniqueType(int orTechnique) { m_technique_type = orTechnique; }

	// Component을(를) 통해 상속됨
	virtual void Enable() override;
	virtual void Disable() override;
protected:
	void SetMaterials(vector<GeneralMaterial>& materialSrc);
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