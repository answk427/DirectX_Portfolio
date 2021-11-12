#include "ObjectMgr.h"

int ObjectMgr::idNumber = 1;

GameObject& ObjectMgr::CreateGameObject(const gameObjectID& id)
{
	if (gameObjects.find(id) != gameObjects.end())
	{
		return gameObjects[id];
	}
	auto it = gameObjects.insert(std::make_pair(id, GameObject(id)));
		
	return (it.first->second);
}

GameObject & ObjectMgr::CreateGameObject(const gameObjectID& id, const std::wstring & name)
{
	if (gameObjects.find(id) != gameObjects.end())
	{
		return gameObjects[id];
	}
	auto it = gameObjects.insert(std::make_pair(id, GameObject(id, name)));
	
	
	return (it.first->second);
}

GameObject & ObjectMgr::CreateGameObject(const gameObjectID& id, const std::wstring & name, GameObject* parent)
{
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

Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name)
{
	
	Mesh* mesh = meshMgr.CreateMeshFromFile(name, assimpMesh);
	Renderer* renderer;
	
	//뼈가 없는 구조
	if (!assimpMesh.HasBone())
	{
		MeshRenderer* comp = (MeshRenderer*)componentMgr.CreateComponent(ComponentType::MESHRENDERER);
		comp->SetMesh(mesh);
		comp->SetMaterials(assimpMesh.GetMaterials());

		//init 해야됨
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");
		
		renderer = comp;
	}
	else
	{
		SkinnedMeshRenderer* comp = (SkinnedMeshRenderer*)componentMgr.CreateComponent(ComponentType::SKINNEDMESHRENDERER);
		comp->SetMesh(mesh);
		comp->SetMaterials(assimpMesh.GetMaterials());
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
	GameObject& obj = CreateGameObject(makeID(), name, parent);
	
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
	GameObject& obj = CreateGameObject(makeID(),name);
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
