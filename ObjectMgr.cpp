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

	//�θ��ڽ� ���� ����
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
	//assimpScene�� ���� ���Ϸ� �ʱ�ȭ �� �������� Ȯ��
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
	
	//�ε��� �ִϸ��̼� ����
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

	//������ ������Ʈ�� �ڽĿ�����Ʈ�� ����
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
	//��� ���� ���ؼ� Ž��
	for (int i=0; i< boneCount; ++i)
	{
		auto& frame = assimpAni.HierarchyAniClip[i];
		int frameSize = frame.scalingKey.size();
		std::vector<Keyframe> Keyframes(frameSize);
		
		//�� ������ �ð������� ��ġ���� ��ȸ�ϸ鼭 ����
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

	//���� ���� ����
	if (!assimpMesh.HasBone())
	{
		MeshRenderer* comp = (MeshRenderer*)componentMgr->CreateComponent(ComponentType::MESHRENDERER, ownerId);
		comp->SetMesh(mesh, assimpMesh.GetMaterials());

		//init �ؾߵ�
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");

		renderer = comp;
	}
	//���� �ִ� ����
	else
	{
		SkinnedMeshRenderer* comp = (SkinnedMeshRenderer*)componentMgr->CreateComponent(ComponentType::SKINNEDMESHRENDERER, ownerId);
		comp->SetMesh(mesh, assimpMesh.GetMaterials());

		//init �ؾߵ�
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");

		BoneDatas boneDatas;
		boneDatas.SetParents(m_finalHierarchy->parents);
		boneDatas.SetOffsets(m_finalHierarchy->offsets);
		boneDatas.SetParentMatrix(m_finalHierarchy->boneParentMatrix);
		comp->SetBoneDatas(boneDatas);

		//������ �� �ε���,����ġ ��Ű�� ������ ���� ���
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

	//���ϰ�ο��� ���ϸ� ����
	/*int pos = fileName.rfind('/');
	std::string name = fileName.substr(pos + 1);*/

	//A2W(�����ڵ� -> ��Ƽ����Ʈ ��ȯ �Լ�)�� ����ϱ����� ��ũ��
	/*USES_CONVERSION;
	GameObject& obj = CreateGameObject(A2W(name.c_str()));*/

	//parent�� �ڽĿ�����Ʈ�� ����
	GameObject& obj = CreateGameObject(name, parent);

	//����� �⺻ ���(toParent���?) ����
	XMVECTOR scale, quat, trans;
	XMMatrixDecompose(&scale, &quat, &trans, XMLoadFloat4x4(&node.GetMatrix()));

	obj.transform->SetScale(XMVectorGetX(scale), XMVectorGetY(scale), XMVectorGetZ(scale));
	obj.transform->SetRotation({ XMVectorGetX(quat), XMVectorGetY(quat), XMVectorGetZ(quat), XMVectorGetW(quat) });
	obj.transform->SetPosition(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));


	//bone ������ �ִ� node�� ��� offset �߰�
	auto boneIt = m_finalHierarchy->m_boneNameIdx.find(name);
	int idx;
	if (boneIt != m_finalHierarchy->m_boneNameIdx.end())
	{
		idx = boneIt->second;
		obj.AddBoneOffset(m_finalHierarchy->offsets[idx]);
	}


	//Mesh�� �ִ� Node�� ��� Renderer������Ʈ ���� �� ������Ʈ�� �߰�
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()), obj.GetID());
		comp->SetTransform(obj.transform.get());
		comp->SetNodeHierarchy(obj.nodeHierarchy);
		obj.AddComponent(comp);
	}

	//��� ���������� Ž��
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

	//bone ������ �ִ� node�� ��� offset �߰�
	auto boneIt = m_finalHierarchy->m_boneNameIdx.find(name);
	int idx;
	if (boneIt != m_finalHierarchy->m_boneNameIdx.end())
	{
		idx = boneIt->second;
		obj.AddBoneOffset(m_finalHierarchy->offsets[idx]);
	}



	//Mesh�� �ִ� Node�� ��� Renderer������Ʈ ���� �� ������Ʈ�� �߰�
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()), obj.GetID());
		comp->SetTransform(obj.transform.get());
		comp->SetNodeHierarchy(obj.nodeHierarchy);
		obj.AddComponent(comp);
	}

	//��� ���������� Ž��
	for (NodeStruct& node : node.childs)
		AddNode(&obj, node);

	return &obj;
}
