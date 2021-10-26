#include "ObjectMgr.h"

GameObject& ObjectMgr::CreateGameObject(std::wstring name)
{
	gameObjects.push_back(GameObject(name));
	return gameObjects[gameObjects.size() - 1];
}

void ObjectMgr::CreateObjectFromFile(std::string fileName)
{
	//assimpScene�� ���� ���Ϸ� �ʱ�ȭ �� �������� Ȯ��
	if (!assimpLoader.EqualFileName(fileName))
	{
		assimpLoader.InitScene(fileName);
		if (!assimpLoader.LoadData())
		{
			return;
		}
			
	}

	Mesh* mesh = meshMgr.CreateMeshFromFile(fileName, assimpLoader);

	//���ϰ�ο��� ���ϸ� ����
	int pos = fileName.rfind('/');
	std::string name = fileName.substr(pos + 1);

	//A2W(�����ڵ� -> ��Ƽ����Ʈ ��ȯ �Լ�)�� ����ϱ����� ��ũ��
	USES_CONVERSION;
	GameObject& obj = CreateGameObject(A2W(name.c_str()));

	
	if (!assimpLoader.HasBone())
	{
		MeshRenderer* comp = (MeshRenderer*)componentMgr.CreateComponent(ComponentType::MESHRENDERER);
		comp->SetMesh(mesh);
		comp->SetMaterials(assimpLoader.GetMaterials());
		
		//init �ؾߵ�
		comp->InitDiffuseMaps(textureMgr, L"Textures/");
		comp->InitEffects(effectMgr, L"FX/");
		
		//...

		comp->SetTransform(&obj.transform);
		obj.AddComponent(comp);

	}
	else
	{
		SkinnedMeshRenderer* comp = (SkinnedMeshRenderer*)componentMgr.CreateComponent(ComponentType::SKINNEDMESHRENDERER);
		comp->SetMesh(mesh);
		comp->SetMaterials(assimpLoader.GetMaterials());
		//init �ؾߵ�
		//...

		comp->SetTransform(&obj.transform);
		obj.AddComponent(comp);
	}
}
