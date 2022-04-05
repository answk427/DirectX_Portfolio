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
	
	if (root == nullptr)
		return nullptr;
	
	
	return AddNode(*root);
}

GameObject & ObjectMgr::CreateBasicBoxObject()
{
	GameObject& obj = CreateGameObject(L"BoxObject");
	Mesh* mesh = meshMgr->CreateBasicBox(2, 2, 2);
	Renderer* renderer = dynamic_cast<Renderer*>(componentMgr->CreateComponent(ComponentType::MESHRENDERER));
	
	renderer->SetMesh(mesh);
	renderer->SetTransform(&obj.transform);
	//renderer->MapsInit();
	//renderer->InitEffects();
	
	obj.AddComponent(renderer);
	
	return obj;
}

GameObject & ObjectMgr::CreateBasicGrid()
{
	GameObject& obj = CreateGameObject(L"GridObject");
	Mesh* mesh = meshMgr->CreateBasicGrid(10, 10, 2, 2);
	Renderer* renderer = dynamic_cast<Renderer*>(componentMgr->CreateComponent(ComponentType::MESHRENDERER));

	renderer->SetMesh(mesh);
	renderer->SetTransform(&obj.transform);
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
		component = componentMgr->CreateComponent(compType);
		((Lighting*)component)->transform = &obj->transform;
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

Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name)
{
	
	Mesh* mesh = meshMgr->CreateMeshFromFile(name, assimpMesh);
	Renderer* renderer;
	
	//뼈가 없는 구조
	if (!assimpMesh.HasBone())
	{
		MeshRenderer* comp = (MeshRenderer*)componentMgr->CreateComponent(ComponentType::MESHRENDERER);
		comp->SetMesh(mesh, assimpMesh.GetMaterials());
		
		//init 해야됨
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");
		
		renderer = comp;
	}
	else
	{
		SkinnedMeshRenderer* comp = (SkinnedMeshRenderer*)componentMgr->CreateComponent(ComponentType::SKINNEDMESHRENDERER);
		comp->SetMesh(mesh, assimpMesh.GetMaterials());
		
		//init 해야됨
		//...
		renderer = comp;
	}
	return renderer;
}



gameObjectID ObjectMgr::makeID()
{
	gameObjectID id = PRETEXTID + std::to_string(idNumber++);
	
	return id;
}

void ObjectMgr::AddNode(GameObject* parent,NodeStruct& node)
{
	
	wstring name = node.GetName();
	
	//파일경로에서 파일명만 추출
	/*int pos = fileName.rfind('/');
	std::string name = fileName.substr(pos + 1);*/

	//A2W(유니코드 -> 멀티바이트 변환 함수)를 사용하기위한 매크로
	/*USES_CONVERSION;
	GameObject& obj = CreateGameObject(A2W(name.c_str()));*/

	//parent의 자식오브젝트로 생성
	GameObject& obj = CreateGameObject(name, parent);
	
	obj.transform.SetPosition(0, 0, 0);
	
		
	//Mesh가 있는 Node일 경우 Renderer컴포넌트 생성 후 오브젝트에 추가
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()));
		comp->SetTransform(&obj.transform);
		obj.AddComponent(comp);
	}

	//모든 하위노드들을 탐색
	for (NodeStruct& node : node.childs)
		AddNode(&obj, node);
	
	return;
}

GameObject * ObjectMgr::AddNode(NodeStruct & node)
{
	wstring name = node.GetName();
	GameObject& obj = CreateGameObject(name);
	obj.transform.SetPosition(0, 0, 0);

	//Mesh가 있는 Node일 경우 Renderer컴포넌트 생성 후 오브젝트에 추가
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()));
		comp->SetTransform(&obj.transform);
		obj.AddComponent(comp);
	}

	//모든 하위노드들을 탐색
	for (NodeStruct& node : node.childs)
		AddNode(&obj, node);
		
	return &obj;
}
