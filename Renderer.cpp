#include "Renderer.h"
#include "RayPicking.h"

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

void Renderer::AddInstancingQueue()
{
	mesh->InstancingDatas.push_back(new InstancingData());
	m_instancingIdx = mesh->InstancingDatas.size() - 1;
	
	for (int i = 0; i < materials.size(); ++i)
	{
		//i번째 subset에 해당하는 diffuseMap 이름을 설정
		mesh->textureNames[i].push_back(materials[i].diffuseMapName);
	}
	//texture배열의 사이즈를 변경하거나 새로생성
	TextureMgr& textureMgr = TextureMgr::Instance();
	mesh->CreateTextureArrayResourceView(textureMgr.md3dDevice, textureMgr.m_context);
}

void Renderer::InstancingUpdate()
{
	//instancing 중이 아니면 return
	if (!GetInstancing() || mesh == nullptr)
		return;
	
	//세계행렬 업데이트
	//mesh->InstancingDatas[m_instancingIdx]->world = transform->m_world;
	GetWorldMatrix(mesh->InstancingDatas[m_instancingIdx]->world);
	
	//역전치행렬 업데이트
	XMStoreFloat4x4(&mesh->InstancingDatas[m_instancingIdx]->worldInvTranspose,
		MathHelper::InverseTranspose(
			XMLoadFloat4x4(&mesh->InstancingDatas[m_instancingIdx]->world)));
	//rgb 업데이트
	mesh->InstancingDatas[m_instancingIdx]->color = m_color;
	//현재 렌더러를 알려주는 인덱스
	mesh->InstancingDatas[m_instancingIdx]->RendererIdx = m_instancingIdx;

	//이번 프레임에 렌더링할 오브젝트로 등록
	mesh->enableInstancingIndexes.push_back(m_instancingIdx);
}

void Renderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (mesh == nullptr)
		return;

	//XMMATRIX world = XMLoadFloat4x4(&transform->m_world);
	XMMATRIX world;
	GetWorldMatrix(world);
		
	
	//정점버퍼, 인덱스버퍼를 입력조립기에 묶음
	if (GetInstancing())
	{
		//이미 렌더링을 했거나, 그릴 오브젝트들이 없는 경우 리턴
		if (mesh->enableInstancingIndexes.empty())
			return;
		mesh->InstancingUpdate(context);
		mesh->SetInstanceVB(context);
	}
	else
		SetVB(context);
	

	mesh->SetIB(context);

	int subsetLength = mesh->GetSubsetLength();


	ID3DX11EffectTechnique* activeTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	for (int i = 0; i < subsetLength; i++)
	{
		//쉐이더 별로 파이프라인 설정
		if (i == 0 || (effects[i - 1] != effects[i])) //연속으로 같은 쉐이더는 설정하지않음
		{
			//입력조립기 세팅
			if (GetInstancing())
			{
				if (!effects[i]->IASetting(context, m_technique_type | TechniqueType::Instancing))
					break;
			}
			else
			{
				if(!effects[i]->IASetting(context, m_technique_type))
					break;
			}
			//출력병합기 세팅
			if (!effects[i]->OMSetting(context, m_blending))
				break;
		}

		UINT tempTechType = m_technique_type;
		if (isShadowRender())
			tempTechType = tempTechType | TechniqueType::Shadowed;
		
		
		if(GetInstancing())
			activeTech = effects[i]->GetTechnique(tempTechType | TechniqueType::Instancing);
		else
			activeTech = effects[i]->GetTechnique(tempTechType);

		activeTech->GetDesc(&techDesc);
		


		//shader에 필요한 데이터 설정
		effects[i]->PerObjectSet(&materials[i],
			camera, world);

		//인스턴싱일 때 텍스쳐배열 사용
		if(GetInstancing())
			effects[i]->SetMapArray(mesh->textureArrays[i]);
		else
			effects[i]->SetMaps(diffuseMaps[i], normalMaps[i], nullptr);



		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			activeTech->GetPassByIndex(p)->Apply(0, context);
						
			mesh->Draw(context, i);
		}
	}
	//instancing 렌더링은 Draw 호출 한번만 하도록 벡터를 비워줌.
	if (GetInstancing())
		mesh->enableInstancingIndexes.clear();
	
	//현재 렌더링이 그림자맵 렌더링이면
	if (isShadowMapRender())
		isRenderShadowMapBaking = false;
}

Renderer::Renderer(const std::string& id, ComponentType type, const gameObjectID& ownerObj) : Component(id,type, ownerObj)
, m_texMgr(TextureMgr::Instance()), m_effectMgr(EffectMgr::Instance()), m_color(1.0f,1.0f,1.0f,1.0f),
mesh(0), m_blending(0), m_technique_type(TechniqueType::Light), m_instancingIdx(0), m_octreeData({ 0,0 }), isRenderShadowMapBaking(0)
{
	MapsInit();
	SetMaterials(std::vector<GeneralMaterial>(1));
	InitEffects();
}

Renderer::Renderer(const std::string & id, ComponentType type, const gameObjectID& ownerObj, Mesh * mesh) : Component(id, type, ownerObj)
, m_texMgr(TextureMgr::Instance()), m_effectMgr(EffectMgr::Instance()), m_color(1.0f, 1.0f, 1.0f, 1.0f),
mesh(0), m_blending(0), m_technique_type(TechniqueType::Light), m_instancingIdx(0), m_octreeData({ 0,0 }), isRenderShadowMapBaking(0)
{
	SetMesh(mesh);
	SetMaterials(std::vector<GeneralMaterial>(1));
	MapsInit();
	InitEffects();
}

Renderer::~Renderer()
{

}

Renderer & Renderer::operator=(const Renderer & other)
{
	Component::operator=(other);
	transform = other.transform;
	mesh = other.mesh;
	materials = other.materials;
	diffuseMaps = other.diffuseMaps;
	normalMaps = other.normalMaps;
	effects = other.effects;
	m_texMgr = other.m_texMgr;
	m_effectMgr = other.m_effectMgr;

	m_technique_type = other.m_technique_type;
	m_blending = other.m_blending;
	m_instancingIdx = other.m_instancingIdx;
	m_static = other.m_static;
	m_color = other.m_color;
	m_octreeData = other.m_octreeData;

	isShadowed = other.isShadowed;
	isShadowBaking = other.isShadowBaking;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

void Renderer::InitDiffuseMaps(TextureMgr& texMgr, const std::wstring& texturePath)
{
	diffuseMaps.clear();
	for (auto& elem : materials)
	{	
		ID3D11ShaderResourceView* srv = nullptr;
		if(!elem.diffuseMapName.empty())
			srv= texMgr.CreateTexture(texturePath + elem.diffuseMapName);
		diffuseMaps.push_back(srv);
	}
}

void Renderer::InitDiffuseMaps()
{
	diffuseMaps.clear();
	for (auto& elem : materials)
	{
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(elem.diffuseMapName);
		diffuseMaps.push_back(srv);
	}
}

void Renderer::ModifyDiffuseMap(int materialIdx, const std::wstring & mapName, UINT mapType)
{
	TextureMgr& textureMgr = TextureMgr::Instance();
	if (mapType == 0)
	{
		materials[materialIdx].diffuseMapName = mapName;
		
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(mapName);
		diffuseMaps[materialIdx] = srv;

		mesh->ModifyTextureArraySubResource(textureMgr.md3dDevice, textureMgr.m_context,
			m_instancingIdx, materialIdx, mapName);
	}
	else if (mapType == 1)
	{
		materials[materialIdx].normalMapName = mapName;
		
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(mapName);
		normalMaps[materialIdx] = srv;
	}
		
}

void Renderer::InitNormalMaps(TextureMgr& texMgr, const std::wstring& texturePath)
{
	normalMaps.clear();
	for (auto& elem : materials)
	{
		ID3D11ShaderResourceView* srv = nullptr;
		if (!elem.diffuseMapName.empty())
			srv = texMgr.CreateTexture(texturePath + elem.diffuseMapName);
		
		normalMaps.push_back(srv);
	}
}

void Renderer::InitNormalMaps()
{
	normalMaps.clear();
	for (auto& elem : materials)
	{
		ID3D11ShaderResourceView* srv = m_texMgr.CreateTexture(elem.normalMapName);
		normalMaps.push_back(srv);
	}
}

void Renderer::InitEffects(EffectMgr & effectMgr, const std::wstring& shaderPath)
{
	effects.clear();
	for (auto& elem : materials)
	{
		//Effect* effect = effectMgr.GetEffect(shaderPath + elem.ShaderName);
		Effect* effect = effectMgr.GetEffect(shaderPath + L"Basic.fxo");
		effects.push_back(effect);
	}		
}

void Renderer::InitEffects()
{
	effects.clear();
	for (auto& elem : materials)
	{
		//Effect* effect = effectMgr.GetEffect(shaderPath + elem.ShaderName);
		//Effect* effect = m_effectMgr.GetEffect(elem.ShaderName);
		Effect* effect = m_effectMgr.GetEffect(L"FX/Basic.fxo");
		effects.push_back(effect);
	}
}

void Renderer::InitEffects(const std::vector<std::wstring>& shaderNames, std::vector<EffectType>& effectTypes)
{
	effects.clear();

	UINT subsetSize = mesh->GetSubsetLength();
	
	for (int i=0; i<shaderNames.size(); ++i)
	{		
		m_effectMgr.SetType(shaderNames[i], effectTypes[i]);
		Effect* effect = m_effectMgr.GetEffect(shaderNames[i]);
		assert(effect!=nullptr);
		effects.push_back(effect);
	}
	
	for (int i = shaderNames.size(); i < subsetSize; ++i)
	{
		Effect* effect = m_effectMgr.GetEffect(shaderNames[0]);
		assert(effect != nullptr);
		effects.push_back(effect);
	}
}

void Renderer::SetMesh(Mesh * meshSrc)
{
	//원래 설정되어 있던 mesh의 InstancingData에서 현재 오브젝트의 데이터를 지움.
	if (mesh != nullptr)
	{
		if (mesh->InstancingDatas[m_instancingIdx] != nullptr)
			delete mesh->InstancingDatas[m_instancingIdx];
		mesh->InstancingDatas[m_instancingIdx] = nullptr;
	}

	mesh = meshSrc;

	//Material을 Subset 개수만큼 기본으로 설정
	materials.clear();
	materials.assign(mesh->GetSubsetLength(), GeneralMaterial());

	AddInstancingQueue();
}

void Renderer::SetMesh(Mesh * meshSrc, std::vector<GeneralMaterial>& materialSrc)
{
	//원래 설정되어 있던 mesh의 InstancingData에서 현재 오브젝트의 데이터를 지움.
	if (mesh != nullptr)
	{
		if (mesh->InstancingDatas[m_instancingIdx] != nullptr)
			delete mesh->InstancingDatas[m_instancingIdx];
		mesh->InstancingDatas[m_instancingIdx] = nullptr;
	}

	mesh = meshSrc;

	SetMaterials(materialSrc);
	AddInstancingQueue();
	
}

void Renderer::MapsInit()
{
	InitDiffuseMaps();
	InitNormalMaps();
}

void Renderer::SetMaterials(std::vector<GeneralMaterial>& materialSrc)
{
	//materials를 빈 벡터로 만듬
	std::vector<GeneralMaterial>().swap(materials);
	//매개변수 벡터와 교환
	materials.swap(materialSrc);
	MapsInit();
}

void Renderer::Enable()
{
}

void Renderer::Disable()
{
}


//***********************MeshRenderer **************************//

MeshRenderer::MeshRenderer(const std::string& id, const gameObjectID& ownerId) : Renderer(id, ComponentType::MESHRENDERER, ownerId)
{
	SetTechniqueType(TechniqueType::Light | TechniqueType::DiffuseMap);
}


MeshRenderer & MeshRenderer::operator=(const MeshRenderer & other)
{
	Renderer::operator=(other);

	return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
}

void MeshRenderer::SetNodeHierarchy(std::weak_ptr<NodeHierarchy> bones)
{
	m_bones = bones;
	
	std::shared_ptr<Animator> animator = m_bones.lock()->m_animator;
	
}



SkinnedMeshRenderer::SkinnedMeshRenderer(const std::string & id, const gameObjectID& ownerObj) :
	Renderer(id, ComponentType::SKINNEDMESHRENDERER, ownerObj), 
	m_timer(GameTimer::Instance()), 
	mSkinningComputeShader(std::make_unique<SkinningComputeShader>(L"FX/SkinningCompute.fxo", m_texMgr.md3dDevice))
{
	SetTechniqueType(TechniqueType::Light | TechniqueType::DiffuseMap);
}

SkinnedMeshRenderer::SkinnedMeshRenderer(const SkinnedMeshRenderer & other) : SkinnedMeshRenderer(other.id, other.ownerObjectId)
{
	m_timer = GameTimer::Instance();
	boneDrawMode = other.boneDrawMode;
	//m_animator = other.m_animator;
	if(other.mBoneRenderer)
		*mBoneRenderer.get() = *other.mBoneRenderer.get();
}
//
//SkinnedMeshRenderer::SkinnedMeshRenderer(SkinnedMeshRenderer && other) : SkinnedMeshRenderer(other.id, other.ownerObjectId)
//{
//	m_timer = GameTimer::Instance();
//	boneDrawMode = other.boneDrawMode;
//	m_animator = other.m_animator;
//	SetBoneDatas(other.m_animator->boneDatas);
//}
//
SkinnedMeshRenderer & SkinnedMeshRenderer::operator=(const SkinnedMeshRenderer & skinRenderer)
{
	Renderer::operator=(skinRenderer);
	m_timer = GameTimer::Instance();
	boneDrawMode = skinRenderer.boneDrawMode;
	//m_animator = skinRenderer.m_animator;
	*mBoneRenderer.get() = *skinRenderer.mBoneRenderer.get();

	return *this;
	// TODO: 여기에 반환 구문을 삽입합니다. 
}

void SkinnedMeshRenderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (boneDrawMode)
	{
		mBoneRenderer->Draw(context, camera);
		return;
	}
	if (mesh == nullptr)
		return;
	if ((isShadowMapRender() && GetInstancing() && shadowSkinningInstancingSize == 0))
	{
		isRenderShadowMapBaking = false;
		return;
	}
	
	//정점버퍼, 인덱스버퍼를 입력조립기에 묶음
	if (GetInstancing()) //&& !isShadowMapRender())
	{
		//이미 렌더링을 했거나, 그릴 오브젝트들이 없는 경우 리턴
		if (mesh->enableInstancingIndexes.empty())
		{
			std::shared_ptr<Animator> animator = m_bones.lock()->m_animator;
			animator->SetAnimatedFlag(false);
			return;
		}
			
		if(!isShadowMapRender())
			mesh->InstancingBasicUpdate(context);
		mesh->SetInstanceSkinnedVB(context);
	}
	else
	{
		//skinning이 아닐땐 정점버퍼 바인딩
		//skinning일 경우 나중에 정점데이터 structuredBuffer 바인딩
		if (!(GetTechniqueType() & TechniqueType::Skinned))
			mesh->SetSkinnedVB(context);
	}

	mesh->SetIB(context);
	
	//XMMATRIX world = XMLoadFloat4x4(&transform->m_world);
	XMMATRIX world;
	GetWorldMatrix(world);
	
	std::shared_ptr<Animator> animator = m_bones.lock()->m_animator;
	//그림자맵 렌더링 일때는 정점을 다시 계산할 필요가 없음
	if (!isShadowMapRender())
	{
		//계산쉐이더를 통해 정점스키닝 계산
		if (GetInstancing())
		{
			mesh->m_instancingComputeShader->Execute(context, camera);
		}
		else
		{
			mSkinningComputeShader->Execute(context, camera, world,
				animator->boneDatas.m_finalTransforms);
		}
		
	}
	animator->SetAnimatedFlag(false);

	//ID3D11Buffer* output;
	//D3D11_BUFFER_DESC rwVertexDesc;
	//rwVertexDesc.Usage = D3D11_USAGE_STAGING;
	////rwVertexDesc.ByteWidth = sizeof(MyVertex::ComputedVertex)* mSkinningComputeShader->bufferSize;
	//rwVertexDesc.ByteWidth = sizeof(MyVertex::ComputedVertex)* mesh->m_instancingComputeShader->bufferSize;
	//rwVertexDesc.BindFlags = 0;
	//rwVertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//rwVertexDesc.StructureByteStride = sizeof(MyVertex::ComputedVertex);
	//rwVertexDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//HR(m_texMgr.md3dDevice->CreateBuffer(&rwVertexDesc, 0, &output));

	////context->CopyResource(output, mSkinningComputeShader->RWvertexBuffer);
	//context->CopyResource(output, mesh->m_instancingComputeShader->RWvertexBuffer);

	//D3D11_MAPPED_SUBRESOURCE mappedData;
	//context->Map(output, 0, D3D11_MAP_READ, 0, &mappedData);
	//MyVertex::ComputedVertex* debugVertices =
	//	reinterpret_cast<MyVertex::ComputedVertex*>(mappedData.pData);

	//ReleaseCOM(output);
	

	int subsetLength = mesh->GetSubsetLength();


	ID3DX11EffectTechnique* activeTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	for (int i = 0; i < subsetLength; i++)
	{
		//쉐이더 별로 파이프라인 설정
		if (i == 0 || (effects[i - 1] != effects[i])) //연속으로 같은 쉐이더는 설정하지않음
		{
			//입력조립기 세팅
			if (GetInstancing())
			{
				if (!effects[i]->IASetting(context, GetTechniqueType() |
					TechniqueType::Instancing))
					break;
				effects[i]->SetVertexBufferLen(mSkinningComputeShader->MeshVerticesSize);
			}
			else
			{
				if (!effects[i]->IASetting(context, GetTechniqueType()))
					break;
			}
			//출력병합기 세팅
			if (!effects[i]->OMSetting(context, m_blending))
				break;
		}

		UINT tempTechType = GetTechniqueType();
		
		
		if (isShadowRender())
			tempTechType = tempTechType | TechniqueType::Shadowed;
		if (normalMaps[i] != nullptr)
			tempTechType = tempTechType | TechniqueType::NormalMap;


		if (GetInstancing())
			activeTech = effects[i]->GetTechnique(tempTechType | TechniqueType::Instancing);
		else
			activeTech = effects[i]->GetTechnique(tempTechType);

		activeTech->GetDesc(&techDesc);



		//shader에 필요한 데이터 설정
		effects[i]->PerObjectSet(&materials[i],
			camera, world);

		//인스턴싱일 때 텍스쳐배열 사용
		if (GetInstancing())
			effects[i]->SetMapArray(mesh->textureArrays[i]);
		else
			effects[i]->SetMaps(diffuseMaps[i], normalMaps[i], nullptr);

		//BoneMatrix 설정
		/*effects[i]->SetBoneTransforms(&animator->boneDatas.m_finalTransforms[0],
			animator->boneDatas.m_finalTransforms.size());*/
		
		
		//스키닝일 경우 정점버퍼 대신 구조적버퍼로 정점데이터 전달
		if (tempTechType & TechniqueType::Skinned)
		{
			if (GetInstancing())
				effects[i]->SetVertexSrcData(
					mesh->m_instancingComputeShader->computedVertexSRV);
			else
				effects[i]->SetVertexSrcData(mSkinningComputeShader->computedVertexSRV);
		}

		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			activeTech->GetPassByIndex(p)->Apply(0, context);

			if (isShadowMapRender() && GetInstancing())
			{
				//인스턴싱 렌더링
				context->DrawIndexedInstanced(mesh->subsets[i].IndexCount
					, shadowSkinningInstancingSize
					, mesh->subsets[i].IndexStart
					, mesh->subsets[i].VertexStart
					, 0);
			}
			else
				mesh->Draw(context, i);		
		}
	}

	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	context->VSSetShaderResources(0, 1, nullSRV);
		
	//instancing 렌더링은 Draw 호출 한번만 하도록 벡터를 비워줌.
	if (GetInstancing() && !isShadowMapRender())
	{
		shadowSkinningInstancingSize = mesh->enableInstancingIndexes.size();
		mesh->enableInstancingIndexes.clear();
	}
		

	//현재 렌더링이 그림자맵 렌더링이면
	if (isShadowMapRender())
	{
		isRenderShadowMapBaking = false;
		shadowSkinningInstancingSize = 0;
	}
		
}

void SkinnedMeshRenderer::Update()
{
	m_bones.lock()->m_animator->Update(m_timer.DeltaTime());
	
	if (boneDrawMode)
		mBoneRenderer->Update();	
}

void SkinnedMeshRenderer::InstancingUpdate()
{
	//instancing 중이 아니면 return
	if (!GetInstancing() || mesh == nullptr)
		return;

	//세계행렬 업데이트
	GetWorldMatrix(mesh->InstancingDatas[m_instancingIdx]->world);

	//역전치행렬 업데이트
	XMStoreFloat4x4(&mesh->InstancingDatas[m_instancingIdx]->worldInvTranspose,
		MathHelper::InverseTranspose(
			XMLoadFloat4x4(&mesh->InstancingDatas[m_instancingIdx]->world)));
	//rgb 업데이트
	mesh->InstancingDatas[m_instancingIdx]->color = m_color;
	//현재 렌더러를 알려주는 인덱스
	mesh->InstancingDatas[m_instancingIdx]->RendererIdx = m_instancingIdx;

	//이번 프레임에 렌더링할 오브젝트로 등록
	mesh->enableInstancingIndexes.push_back(m_instancingIdx);
	mesh->m_instancingComputeShader->InstancingUpdate(m_instancingIdx);
}

void SkinnedMeshRenderer::InitSkinnedVB()
{
	mesh->InitSkinnedVB(m_texMgr.md3dDevice, m_skinnedDatas);
	mSkinningComputeShader->InitSkinDataBuffer(m_skinnedDatas, m_texMgr.md3dDevice);
	mesh->m_instancingComputeShader->InitSkinDataBuffer(m_skinnedDatas, m_texMgr.md3dDevice);
}

std::vector<std::string> SkinnedMeshRenderer::GetAnimationClipNames()
{
	if (m_bones.expired())
		return std::vector<std::string>();
	
	std::vector<std::string> clipNames;

	std::shared_ptr<Animator> animator = m_bones.lock()->m_animator;
	for (auto& clip : animator->clips)
		clipNames.push_back(clip.first);
		
	return clipNames;
}

void SkinnedMeshRenderer::SetBoneDatas(BoneDatas& boneDatas)
{
	std::shared_ptr<NodeHierarchy> nodeHierarchy = m_bones.lock();
	//animator가 있을경우 바로 본데이터 적재
	if (nodeHierarchy)
	{
		std::shared_ptr<Animator> animator = nodeHierarchy->m_animator;
		if (animator)
		{
			animator->SetBoneDatas(boneDatas);
			mBoneRenderer->SetMesh();
			readBoneData = true;
			return;
		}
	}

	//애니메이터 없을경우 임시저장
	tempBoneDatas = boneDatas;
	readBoneData = false;
}

void SkinnedMeshRenderer::SetNodeHierarchy(std::weak_ptr<NodeHierarchy> bones)
{
	m_bones = bones;
	mBoneRenderer = std::make_unique<BoneRenderer>("BoneRenderer", ownerObjectId, bones);

	std::shared_ptr<Animator> animator = m_bones.lock()->m_animator;
	if (animator)
	{
		//아직 boneData를 읽지 않았을때
		if (!readBoneData)
		{
			animator->SetBoneDatas(tempBoneDatas);
			readBoneData = true;
		}
	}
	mBoneRenderer->SetMesh();
	mesh->m_instancingComputeShader->AddInstance<MyVertex::ComputedVertex>
		(m_texMgr.md3dDevice, m_bones);
}

void SkinnedMeshRenderer::SetTechniqueType(int orTechnique)
{
	if (m_bones.expired())
		return;
	m_bones.lock()->mTechType = orTechnique;
}

UINT SkinnedMeshRenderer::GetTechniqueType()
{
	if (m_bones.expired())
		return TechniqueType::Light | TechniqueType::DiffuseMap;
	return m_bones.lock()->mTechType;
}

void SkinnedMeshRenderer::SetMesh(Mesh * meshSrc)
{
	Renderer::SetMesh(meshSrc);
	mesh->InitInstancingComputeShader(L"FX/SkinningCompute.fxo", m_texMgr.md3dDevice);

	mSkinningComputeShader->InitVertexSRV(mesh->GetVertices(), m_texMgr.md3dDevice);
	mSkinningComputeShader->InitVertexUAV<MyVertex::ComputedVertex>(m_texMgr.md3dDevice);
	mSkinningComputeShader->InitComputedVertexSRV(m_texMgr.md3dDevice);

	mesh->m_instancingComputeShader->InitVertexSRV(mesh->GetVertices(), m_texMgr.md3dDevice);
	mesh->m_instancingComputeShader->InitVertexUAV<MyVertex::ComputedVertex>(m_texMgr.md3dDevice);
	mesh->m_instancingComputeShader->InitComputedVertexSRV(m_texMgr.md3dDevice);
	
}

void SkinnedMeshRenderer::SetMesh(Mesh * meshSrc, std::vector<GeneralMaterial>& materialSrc)
{
	Renderer::SetMesh(meshSrc, materialSrc);
	mesh->InitInstancingComputeShader(L"FX/SkinningCompute.fxo", m_texMgr.md3dDevice);

	mSkinningComputeShader->InitVertexSRV(mesh->GetVertices(), m_texMgr.md3dDevice);
	mSkinningComputeShader->InitVertexUAV<MyVertex::ComputedVertex>(m_texMgr.md3dDevice);
	mSkinningComputeShader->InitComputedVertexSRV(m_texMgr.md3dDevice);

	mesh->m_instancingComputeShader->InitVertexSRV(mesh->GetVertices(), m_texMgr.md3dDevice);
	mesh->m_instancingComputeShader->InitVertexUAV<MyVertex::ComputedVertex>(m_texMgr.md3dDevice);
	mesh->m_instancingComputeShader->InitComputedVertexSRV(m_texMgr.md3dDevice);
	
}

void SkinnedMeshRenderer::SetAnimationClip(const std::string & clipName)
{
	/*if (m_animator->ChangeClip(clipName))
		SetTechniqueType(TechniqueType::Light | TechniqueType::DiffuseMap | TechniqueType::Skinned);*/
	if (m_bones.expired())
		return;
	if (m_bones.lock()->m_animator->ChangeClip(clipName))
		SetTechniqueType(TechniqueType::Light | TechniqueType::DiffuseMap | TechniqueType::Skinned);
	else
		SetTechniqueType(TechniqueType::Light | TechniqueType::DiffuseMap);
}

void SkinnedMeshRenderer::LoadAnimationClip(MyAnimationClip & clip)
{
	if (m_bones.expired())
		return;

	std::shared_ptr<Animator> animator = m_bones.lock()->m_animator;
	if (animator)
	{
		animator->LoadAnimationClip(clip);
		SetAnimationClip(clip.m_clipName);
	}
		
}

void SkinnedMeshRenderer::DeleteAnimationClip(const std::string & clipName)
{
	if (m_bones.expired())
		return;

	std::shared_ptr<Animator> animator = m_bones.lock()->m_animator;
	if (animator)
	{
		animator->DeleteAnimation(clipName);
		if (!animator->clips.empty())
			SetAnimationClip(animator->clips.begin()->first);
		else
			SetAnimationClip("");
	}
}



BoneRenderer::BoneRenderer(const std::string& id, const gameObjectID &ownerObj,
	std::weak_ptr<NodeHierarchy> nodes) :
	Renderer(id, ComponentType::MESHRENDERER, ownerObj) 
{	
	SetTechniqueType(TechniqueType::Light);
	SetNodeHierarchy(nodes);
}


void BoneRenderer::CreateBoneShape(std::vector<Vertex::Basic32>& result, XMFLOAT3 & pos, XMFLOAT3 & xAxis, XMFLOAT3 & yAxis, XMFLOAT3 & zAxis)
{
	XMFLOAT3 floorRight, floorFront, floorLeft, floorBack;
	XMFLOAT3 vertexY;
	XMFLOAT3 vertexDownY;

	xAxis.x = xAxis.x * CRYSTALSIZE;
	xAxis.y = xAxis.y * CRYSTALSIZE;
	xAxis.z = xAxis.z * CRYSTALSIZE;

	yAxis.x = yAxis.x * CRYSTALSIZE;
	yAxis.y = yAxis.y * CRYSTALSIZE;
	yAxis.z = yAxis.z * CRYSTALSIZE;

	zAxis.x = zAxis.x * CRYSTALSIZE;
	zAxis.y = zAxis.y * CRYSTALSIZE;
	zAxis.z = zAxis.z * CRYSTALSIZE;

	//각 기저벡터축의 단위벡터만큼 이동
	//중앙 바닥을 이루는 정점들
	floorRight.x = pos.x + xAxis.x;
	floorRight.y = pos.y + xAxis.y;
	floorRight.z = pos.z + xAxis.z;

	floorFront.x = pos.x + zAxis.x;
	floorFront.y = pos.y + zAxis.y;
	floorFront.z = pos.z + zAxis.z;

	floorLeft.x = pos.x - xAxis.x;
	floorLeft.y = pos.y - xAxis.y;
	floorLeft.z = pos.z - xAxis.z;

	floorBack.x = pos.x - zAxis.x;
	floorBack.y = pos.y - zAxis.y;
	floorBack.z = pos.z - zAxis.z;

	//위쪽 꼭지점 정점
	vertexY.x = pos.x + yAxis.x;
	vertexY.y = pos.y + yAxis.y;
	vertexY.z = pos.z + yAxis.z;

	//아래쪽 꼭지점 정점
	vertexDownY.x = pos.x - yAxis.x;
	vertexDownY.y = pos.y - yAxis.y;
	vertexDownY.z = pos.z - yAxis.z;

	//오른쪽부터 시계방향 순서로 저장
	result[0].Pos = floorRight;
	result[1].Pos = floorBack;
	result[2].Pos = floorLeft;
	result[3].Pos = floorFront;
	result[4].Pos = vertexY;
	result[5].Pos = vertexDownY;
	result[6].Pos = pos;
}

void BoneRenderer::InitIndices(std::vector<UINT>& indices)
{
	std::shared_ptr<Animator> m_Animator = m_bones.lock()->m_animator;
	if (!m_Animator)
		return;
	//뼈 하나당 정점이 7개 이므로
	UINT boneSize = vertices.size() / 7;


	//뼈 하나당 
	for (int i = 0; i < boneSize; ++i)
	{
		UINT firstIdx = i * 7;
		int parentIdx = m_Animator->boneDatas.m_parentIndices[i];
		UINT nextVertex = parentIdx * 7;

		//정오면체의 아랫면
		indices.push_back(firstIdx + FLOORRIGHT);
		indices.push_back(firstIdx + FLOORBACK);
		indices.push_back(firstIdx + FLOORFRONT);
		indices.push_back(firstIdx + FLOORBACK);
		indices.push_back(firstIdx + FLOORLEFT);
		indices.push_back(firstIdx + FLOORFRONT);

		//정오면체의 위쪽옆면 4개
		indices.push_back(firstIdx + FLOORRIGHT);
		indices.push_back(firstIdx + UPY);
		indices.push_back(firstIdx + FLOORFRONT);

		indices.push_back(firstIdx + FLOORRIGHT);
		indices.push_back(firstIdx + FLOORBACK);
		indices.push_back(firstIdx + UPY);

		indices.push_back(firstIdx + FLOORBACK);
		indices.push_back(firstIdx + FLOORLEFT);
		indices.push_back(firstIdx + UPY);

		indices.push_back(firstIdx + FLOORLEFT);
		indices.push_back(firstIdx + FLOORFRONT);
		indices.push_back(firstIdx + UPY);

		//정오면체의 아래쪽옆면 4개
		indices.push_back(firstIdx + FLOORRIGHT);
		indices.push_back(firstIdx + FLOORFRONT);
		indices.push_back(firstIdx + DOWNY);

		indices.push_back(firstIdx + FLOORBACK);
		indices.push_back(firstIdx + FLOORRIGHT);
		indices.push_back(firstIdx + DOWNY);

		indices.push_back(firstIdx + FLOORLEFT);
		indices.push_back(firstIdx + FLOORBACK);
		indices.push_back(firstIdx + DOWNY);

		indices.push_back(firstIdx + FLOORFRONT);
		indices.push_back(firstIdx + FLOORLEFT);
		indices.push_back(firstIdx + DOWNY);

		//부모에서 자식 Bone으로 연결하는 인덱스
		if (nextVertex < vertices.size() && nextVertex >=0)
		{
			indices.push_back(nextVertex + FLOORRIGHT);
			indices.push_back(firstIdx + POS);
			indices.push_back(nextVertex + FLOORBACK);
			
			indices.push_back(nextVertex + FLOORBACK);
			indices.push_back(firstIdx + POS);
			indices.push_back(nextVertex + FLOORLEFT);

			indices.push_back(nextVertex + FLOORLEFT);
			indices.push_back(firstIdx + POS);
			indices.push_back(nextVertex + FLOORFRONT);

			indices.push_back(nextVertex + FLOORFRONT);
			indices.push_back(firstIdx + POS);
			indices.push_back(nextVertex + FLOORRIGHT);
		}
	}

}

void BoneRenderer::InitVertices()
{
	std::shared_ptr<Animator>& animator = m_bones.lock()->m_animator;
	if (!animator)
		return;
	vertices.resize(animator->boneDatas.toRoots.size() * CNTPERVERTEX);
}

void BoneRenderer::UpdateVB()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	m_texMgr.m_context->Map(mesh->GetVB(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	Vertex::Basic32* dataView = reinterpret_cast<Vertex::Basic32*>(mappedData.pData);

	for (int i = 0; i < vertices.size(); ++i)
	{
		dataView[i].Pos = vertices[i].Pos;
	}
	m_texMgr.m_context->Unmap(mesh->GetVB(), 0);
}

void BoneRenderer::Update()
{
	static XMVECTOR origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	static XMVECTOR xAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	static XMVECTOR yAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	static XMVECTOR zAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	std::shared_ptr<Animator>& animator = m_bones.lock()->m_animator;
	if (!animator)
		return;
	std::vector<XMFLOAT4X4> toRoots = animator->boneDatas.toRoots;
	UINT boneSize = toRoots.size();
	std::vector<Vertex::Basic32> result(7);

	//기저벡터와 원점을 Root공간에서의 좌표계로 변환함
	XMFLOAT3 resultOrg, resultXaxis, resultYaxis, resultZaxis;
	for (UINT i = 0; i < boneSize; ++i)
	{
		XMMATRIX root = XMLoadFloat4x4(&toRoots[i]);
		XMStoreFloat3(&resultOrg, XMVector4Transform(origin, root));
		XMStoreFloat3(&resultXaxis, XMVector4Transform(xAxis, root));
		XMStoreFloat3(&resultYaxis, XMVector4Transform(yAxis, root));
		XMStoreFloat3(&resultZaxis, XMVector4Transform(zAxis, root));

		CreateBoneShape(result, resultOrg, resultXaxis, resultYaxis, resultZaxis);

		vertices[i * 7] = result[0];
		vertices[i * 7 + 1] = result[1];
		vertices[i * 7 + 2] = result[2];
		vertices[i * 7 + 3] = result[3];
		vertices[i * 7 + 4] = result[4];
		vertices[i * 7 + 5] = result[5];
		vertices[i * 7 + 6] = result[6];
	}
	//정점버퍼의 위치정보 수정
	UpdateVB();
	
	animator->SetAnimatedFlag(false);
}

void BoneRenderer::SetVB(ID3D11DeviceContext * context)
{
	UINT stride[1] = { sizeof(Vertex::Basic32) };
	UINT offset[1] = { 0 };
	ID3D11Buffer* vb = mesh->GetVB();
	context->IASetVertexBuffers(0, 1, &vb, stride, offset);
}

void BoneRenderer::SetMesh()
{
	if (mesh != nullptr)
		delete mesh;
	//bone 구조에 대한 Mesh 생성
	Mesh* tempMesh = new Mesh("BoneRendererMesh");
	InitVertices();
	InitIndices(tempMesh->indices);
	tempMesh->InitWritableVB<MyVertex::BasicVertex>(m_texMgr.md3dDevice, vertices.size());
	tempMesh->InitIB(m_texMgr.md3dDevice);
	tempMesh->InitSingleSubset(vertices);

	Renderer::SetMesh(tempMesh);
	Renderer::MapsInit();
}
