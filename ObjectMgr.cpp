#include "ObjectMgr.h"

int ObjectMgr::idNumber = 1;



void ObjectMgr::Init(MeshMgr* tempMeshMgr, ComponentMgr*  tempComponentMgr)
{
	meshMgr = tempMeshMgr;
	componentMgr = tempComponentMgr;
}

ObjectMgr & ObjectMgr::Instance()
{
	static ObjectMgr* instance = new ObjectMgr();
	return *instance;
}

GameObject& ObjectMgr::CreateGameObject()
{
	gameObjectID id = makeID();

	if (gameObjects.find(id) != gameObjects.end())
	{
		return gameObjects[id];
	}
	auto it = gameObjects.insert(std::make_pair(id, GameObject(id)));

	return (it.first->second);
}

GameObject & ObjectMgr::CreateGameObject(const std::wstring & name)
{
	gameObjectID id = makeID();

	if (gameObjects.find(id) != gameObjects.end())
	{
		return gameObjects[id];
	}
	auto it = gameObjects.insert(std::make_pair(id, GameObject(id, name)));


	return (it.first->second);
}

GameObject & ObjectMgr::CreateGameObject(const std::wstring & name, GameObject* parent)
{
	gameObjectID id = makeID();

	if (gameObjects.find(id) != gameObjects.end())
	{
		return gameObjects[id];
	}
	auto it = gameObjects.insert(std::make_pair(id, GameObject(id, name)));

	//부모자식 관계 설정
	GameObject& result = it.first->second;
	result.SetParent(parent);
	parent->SetChild(&result);


	return result;
}

GameObject*  ObjectMgr::CopyGameObject(GameObject& copyObj)
{
	GameObject& gameObj = CreateGameObject();
	gameObj = copyObj;

	return &gameObj;
}

GameObject* ObjectMgr::CreateObjectFromFile(const std::string& fileName)
{
	//assimpScene이 같은 파일로 초기화 된 상태인지 확인
	if (!assimpLoader.EqualFileName(fileName))
	{
		assimpLoader.InitScene(fileName);
		if (!assimpLoader.LoadData())
		{
			return nullptr;
		}
	}

	NodeStruct* root = assimpLoader.getRoot();
	m_finalHierarchy = assimpLoader.GetHierarchy();
	
	//로딩한 애니메이션 적재
	AnimationManager& aniManager = AnimationManager::Instance();
	m_assimpAnimations = &assimpLoader.getAssimpAnimations();
	aniManager.LoadAnimation(*m_assimpAnimations);
	
	

	if (root == nullptr)
		return nullptr;

	if (!assimpLoader.IsEmptyMesh())
		return AddNode(*root);
	else
		return nullptr;
}

GameObject & ObjectMgr::CreateBasicBoxObject()
{
	GameObject& obj = CreateGameObject(L"BoxObject");
	Mesh* mesh = meshMgr->CreateBasicBox(2, 2, 2);
	Renderer* renderer = dynamic_cast<Renderer*>(componentMgr->CreateComponent(ComponentType::MESHRENDERER, obj.GetID()));

	renderer->SetMesh(mesh);
	renderer->SetTransform(obj.transform.get());
	renderer->SetNodeHierarchy(obj.nodeHierarchy);
	//renderer->MapsInit();
	//renderer->InitEffects();

	obj.AddComponent(renderer);

	return obj;
}

GameObject & ObjectMgr::CreateBasicGrid()
{
	GameObject& obj = CreateGameObject(L"GridObject");
	Mesh* mesh = meshMgr->CreateBasicGrid(10, 10, 2, 2);
	Renderer* renderer = dynamic_cast<Renderer*>(componentMgr->CreateComponent(ComponentType::MESHRENDERER, obj.GetID()));

	renderer->SetMesh(mesh);
	renderer->SetTransform(obj.transform.get());
	renderer->SetNodeHierarchy(obj.nodeHierarchy);
	//renderer->MapsInit();
	//renderer->InitEffects();

	obj.AddComponent(renderer);

	return obj;
}

GameObject & ObjectMgr::CreateTerrain()
{
	GameObject& obj = CreateGameObject(L"TerrainObject");
	Renderer* renderer = dynamic_cast<Renderer*>(componentMgr->CreateComponent(ComponentType::TERRAIN, obj.GetID()));
	renderer->SetTransform(obj.transform.get());
	renderer->SetNodeHierarchy(obj.nodeHierarchy);
	//renderer->MapsInit();
	//renderer->InitEffects();
	obj.AddComponent(renderer);

	return obj;
}

Component * ObjectMgr::AddComponent(GameObject * obj, ComponentType compType)
{
	Component* component = nullptr;
	switch (compType)
	{
	case ComponentType::LIGHT:
		component = componentMgr->CreateComponent(compType, obj->GetID());
		((Lighting*)component)->transform = obj->transform.get();
		break;
	default:
		break;
	}

	if (component == nullptr)
		return nullptr;
	else
	{
		obj->AddComponent(component);
		return component;
	}

}

bool ObjectMgr::DeleteObject(gameObjectID & id)
{
	auto& it = gameObjects.find(id);

	if (it == gameObjects.end())
		return false;

	//삭제한 오브젝트의 자식오브젝트들 삭제
	for (auto& elem : (*it).second.childs)
	{
		DeleteObject(elem->GetID());
	}
	gameObjects.erase(it);

	return true;
}


void ObjectMgr::ConvertOldAnimation(const std::string & clipName, const AssimpAnimation & assimpAni, AnimationClip & dest)
{
	int boneCount = assimpAni.HierarchyAniClip.size();
	dest.BoneAnimations.resize(boneCount);
	//모든 뼈에 대해서 탐색
	for (int i=0; i< boneCount; ++i)
	{
		auto& frame = assimpAni.HierarchyAniClip[i];
		int frameSize = frame.scalingKey.size();
		std::vector<Keyframe> Keyframes(frameSize);
		
		//각 뼈에서 시간에따른 위치정보 순회하면서 저장
		for (int j=0; j< frameSize; ++j)
		{
			Keyframes[j].Scale = frame.scalingKey[j].float3Key;
			Keyframes[j].RotationQuat = frame.quaternionKey[j].quaternionKey;
			Keyframes[j].Translation = frame.positionKey[j].float3Key;
			Keyframes[j].TimePos = frame.positionKey[j].time;				
		}
		dest.BoneAnimations[i].Keyframes.swap(Keyframes);
	}
}

Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name, const gameObjectID& ownerId)
{

	Mesh* mesh = meshMgr->CreateMeshFromFile(name, assimpMesh);
	Renderer* renderer;

	//뼈가 없는 구조
	if (!assimpMesh.HasBone())
	{
		MeshRenderer* comp = (MeshRenderer*)componentMgr->CreateComponent(ComponentType::MESHRENDERER, ownerId);
		comp->SetMesh(mesh, assimpMesh.GetMaterials());

		//init 해야됨
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");

		renderer = comp;
	}
	//뼈가 있는 구조
	else
	{
		SkinnedMeshRenderer* comp = (SkinnedMeshRenderer*)componentMgr->CreateComponent(ComponentType::SKINNEDMESHRENDERER, ownerId);
		comp->SetMesh(mesh, assimpMesh.GetMaterials());

		//init 해야됨
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");

		BoneDatas boneDatas;
		boneDatas.SetParents(m_finalHierarchy->parents);
		boneDatas.SetOffsets(m_finalHierarchy->offsets);
		boneDatas.SetParentMatrix(m_finalHierarchy->boneParentMatrix);
		comp->SetBoneDatas(boneDatas);

		//정점이 뼈 인덱스,가중치 스키닝 정보를 가진 경우
		for (auto& skinnedVertex : assimpMesh.skinnedVertices)
		{
			std::vector<int> indices;
			std::vector<float> weights;
			for (auto idx : skinnedVertex.nodeIndices)
				indices.push_back(idx);
			for (auto weight : skinnedVertex.weights)
				weights.push_back(weight);
			
			comp->m_skinnedDatas.push_back({ weights,indices });
		}
		comp->InitSkinnedVB();
				
		AnimationManager& aniManager = AnimationManager::Instance();
		std::string clipName = "";
		for (auto& clip : *m_assimpAnimations)
		{
			MyAnimationClip* aniClip = 
				aniManager.GetAnimation(clip.first);
									
			comp->LoadAnimationClip(*aniClip);
			
		}
		
		renderer = comp;
	}
	return renderer;
}



gameObjectID ObjectMgr::makeID()
{
	gameObjectID id = PRETEXTID + std::to_string(idNumber++);

	return id;
}

void ObjectMgr::AddNode(GameObject* parent, NodeStruct& node)
{
	std::wstring name = node.GetName();

	//파일경로에서 파일명만 추출
	/*int pos = fileName.rfind('/');
	std::string name = fileName.substr(pos + 1);*/

	//A2W(유니코드 -> 멀티바이트 변환 함수)를 사용하기위한 매크로
	/*USES_CONVERSION;
	GameObject& obj = CreateGameObject(A2W(name.c_str()));*/

	//parent의 자식오브젝트로 생성
	GameObject& obj = CreateGameObject(name, parent);

	//노드의 기본 행렬(toParent행렬?) 추출
	XMVECTOR scale, quat, trans;
	XMMatrixDecompose(&scale, &quat, &trans, XMLoadFloat4x4(&node.GetMatrix()));

	obj.transform->SetScale(XMVectorGetX(scale), XMVectorGetY(scale), XMVectorGetZ(scale));
	obj.transform->SetRotation({ XMVectorGetX(quat), XMVectorGetY(quat), XMVectorGetZ(quat), XMVectorGetW(quat) });
	obj.transform->SetPosition(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));


	//bone 정보가 있는 node일 경우 offset 추가
	auto boneIt = m_finalHierarchy->m_boneNameIdx.find(name);
	int idx;
	if (boneIt != m_finalHierarchy->m_boneNameIdx.end())
	{
		idx = boneIt->second;
		obj.AddBoneOffset(m_finalHierarchy->offsets[idx]);
	}


	//Mesh가 있는 Node일 경우 Renderer컴포넌트 생성 후 오브젝트에 추가
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()), obj.GetID());
		comp->SetTransform(obj.transform.get());
		comp->SetNodeHierarchy(obj.nodeHierarchy);
		obj.AddComponent(comp);
	}

	//모든 하위노드들을 탐색
	for (NodeStruct& node : node.childs)
		AddNode(&obj, node);

	return;
}

GameObject * ObjectMgr::AddNode(NodeStruct & node)
{
	std::wstring name = node.GetName();
	GameObject& obj = CreateGameObject(name);
	XMVECTOR scale, quat, trans;

	XMMatrixDecompose(&scale, &quat, &trans, XMLoadFloat4x4(&node.GetMatrix()));


	obj.transform->SetScale(XMVectorGetX(scale), XMVectorGetY(scale), XMVectorGetZ(scale));
	obj.transform->SetRotation({ XMVectorGetX(quat), XMVectorGetY(quat), XMVectorGetZ(quat), XMVectorGetW(quat) });
	obj.transform->SetPosition(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));

	//bone 정보가 있는 node일 경우 offset 추가
	auto boneIt = m_finalHierarchy->m_boneNameIdx.find(name);
	int idx;
	if (boneIt != m_finalHierarchy->m_boneNameIdx.end())
	{
		idx = boneIt->second;
		obj.AddBoneOffset(m_finalHierarchy->offsets[idx]);
	}



	//Mesh가 있는 Node일 경우 Renderer컴포넌트 생성 후 오브젝트에 추가
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()), obj.GetID());
		comp->SetTransform(obj.transform.get());
		comp->SetNodeHierarchy(obj.nodeHierarchy);
		obj.AddComponent(comp);
	}

	//모든 하위노드들을 탐색
	for (NodeStruct& node : node.childs)
		AddNode(&obj, node);

	return &obj;
}
