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
	
	//�θ��ڽ� ���� ����
	GameObject& result = it.first->second;
	result.SetParent(parent);
	parent->SetChild(&result);


	return result;
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
	
	if (root == nullptr)
		return nullptr;
	
	
	return AddNode(*root);
}

Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name)
{
	
	Mesh* mesh = meshMgr.CreateMeshFromFile(name, assimpMesh);
	Renderer* renderer;
	
	//���� ���� ����
	if (!assimpMesh.HasBone())
	{
		MeshRenderer* comp = (MeshRenderer*)componentMgr.CreateComponent(ComponentType::MESHRENDERER);
		comp->SetMesh(mesh);
		comp->SetMaterials(assimpMesh.GetMaterials());

		//init �ؾߵ�
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");
		
		renderer = comp;
	}
	else
	{
		SkinnedMeshRenderer* comp = (SkinnedMeshRenderer*)componentMgr.CreateComponent(ComponentType::SKINNEDMESHRENDERER);
		comp->SetMesh(mesh);
		comp->SetMaterials(assimpMesh.GetMaterials());
		//init �ؾߵ�
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
	
	//���ϰ�ο��� ���ϸ� ����
	/*int pos = fileName.rfind('/');
	std::string name = fileName.substr(pos + 1);*/

	//A2W(�����ڵ� -> ��Ƽ����Ʈ ��ȯ �Լ�)�� ����ϱ����� ��ũ��
	/*USES_CONVERSION;
	GameObject& obj = CreateGameObject(A2W(name.c_str()));*/

	//parent�� �ڽĿ�����Ʈ�� ����
	GameObject& obj = CreateGameObject(makeID(), name, parent);
	
	obj.transform.SetPosition(0, 0, 0);
	
		
	//Mesh�� �ִ� Node�� ��� Renderer������Ʈ ���� �� ������Ʈ�� �߰�
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()));
		comp->SetTransform(&obj.transform);
		obj.AddComponent(comp);
	}

	//��� ���������� Ž��
	for (NodeStruct& node : node.childs)
		AddNode(&obj, node);
	
	return;
}

GameObject * ObjectMgr::AddNode(NodeStruct & node)
{
	wstring name = node.GetName();
	GameObject& obj = CreateGameObject(makeID(),name);
	obj.transform.SetPosition(0, 0, 0);

	//Mesh�� �ִ� Node�� ��� Renderer������Ʈ ���� �� ������Ʈ�� �߰�
	if (node.assimpMesh != nullptr)
	{
		USES_CONVERSION;
		Renderer* comp = CreateRenderer(*node.assimpMesh, W2A(name.c_str()));
		comp->SetTransform(&obj.transform);
		obj.AddComponent(comp);
	}

	//��� ���������� Ž��
	for (NodeStruct& node : node.childs)
		AddNode(&obj, node);

	
	return &obj;
}
