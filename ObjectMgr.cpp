#include "ObjectMgr.h"

GameObject& ObjectMgr::CreateGameObject(std::wstring name)
{
	gameObjects.push_back(GameObject(name));
	return gameObjects[gameObjects.size() - 1];
}

void ObjectMgr::CreateObjectFromFile(std::string fileName)
{
	//assimpScene이 같은 파일로 초기화 된 상태인지 확인
	if (!assimpLoader.EqualFileName(fileName))
	{
		assimpLoader.InitScene(fileName);
		if (!assimpLoader.LoadData())
		{
			return;
		}
			
	}

	Mesh* mesh = meshMgr.CreateMeshFromFile(fileName, assimpLoader);

	//파일경로에서 파일명만 추출
	int pos = fileName.rfind('/');
	std::string name = fileName.substr(pos + 1);

	//A2W(유니코드 -> 멀티바이트 변환 함수)를 사용하기위한 매크로
	USES_CONVERSION;
	GameObject& obj = CreateGameObject(A2W(name.c_str()));

	
	if (!assimpLoader.HasBone())
	{
		MeshRenderer* comp = (MeshRenderer*)componentMgr.CreateComponent(ComponentType::MESHRENDERER);
		comp->SetMesh(mesh);
		comp->SetMaterials(assimpLoader.GetMaterials());
		
		//init 해야됨
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
		//init 해야됨
		//...

		comp->SetTransform(&obj.transform);
		obj.AddComponent(comp);
	}
}
