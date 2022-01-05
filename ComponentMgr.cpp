#include "ComponentMgr.h"

std::vector<std::string> idFront = { "MESHRENDERER", "SKINNEDMESHRENDERER" , "LIGHT", "DEFAULT" };

ComponentMgr::ComponentMgr() : enableCount_meshRenderer(0), enableCount_skinnedMeshRenderer(0)
, enableCount_lighting(0),creatingIdNum(0)
{
	//�޸� ũ�� �Ҵ�
	meshRenderers.reserve(MESHRENDERERSIZE);
	skinnedMeshRenderers.reserve(SKINNEDMESHRENDERERSIZE);
	lightings.reserve(LIGHTSIZE);
}

ComponentMgr::~ComponentMgr()
{
	for (auto elem : components)
		delete elem;
}

ComponentMgr& ComponentMgr::Instance()
{
	static ComponentMgr* instance = new ComponentMgr();
	return *instance;
}


Component * ComponentMgr::OnOffComponent(Component * component, Command endisable)
{
	ComponentType type = component->componentType;
			
	switch (type)
	{
		case ComponentType::MESHRENDERER:
			return SelectSwap(meshRenderers,
				enableCount_meshRenderer,
				idMap[component->id],
				endisable);

		case ComponentType::SKINNEDMESHRENDERER:
			return SelectSwap(skinnedMeshRenderers,
				enableCount_skinnedMeshRenderer,
				idMap[component->id],
				endisable);
		case ComponentType::LIGHT:
			return SelectSwap(lightings,
				enableCount_lighting,
				idMap[component->id],
				endisable);
		default:
			return component;
	}
}


Component * ComponentMgr::CreateComponent(ComponentType compType)
{
	componentID id = idFront[compType] + "_" + std::to_string(creatingIdNum++);
	switch (compType)
	{
		case ComponentType::MESHRENDERER :
		{
			
			//id���� �� vector�� �߰�
			meshRenderers.push_back(MeshRenderer(id));
			//id�� index ����
			idMap[meshRenderers.back().id] = meshRenderers.size() - 1;
			//Ȱ��ȭ ���·� �ٲ�
			return SelectSwap(meshRenderers, enableCount_meshRenderer,
				meshRenderers.size() - 1, Command::ENABLE);

			
		}
		case ComponentType::SKINNEDMESHRENDERER :
		{
			skinnedMeshRenderers.push_back(
				SkinnedMeshRenderer(id));
			idMap[skinnedMeshRenderers.back().id] = skinnedMeshRenderers.size() - 1;
			return SelectSwap(skinnedMeshRenderers, enableCount_skinnedMeshRenderer,
				skinnedMeshRenderers.size() - 1, Command::ENABLE);
		}
		case ComponentType::LIGHT:
		{
			lightings.push_back(
				Lighting(id));
			idMap[lightings.back().id] = lightings.size() - 1;
			return SelectSwap(lightings, enableCount_lighting,
				lightings.size() - 1, Command::ENABLE);
		}
			
	}
	return nullptr;
}

ComponentType ComponentMgr::typeOfID(componentID & id)
{
	int idx = id.find('_');

	std::string front = id.substr(0, idx);
	
	//ID�� ��ġ�ϴ� ������ƮŸ���� �˻� �� ��ȯ
	for (int i = 0; i < idFront.size(); ++i)
	{
		if (idFront[i] == front)
			return (ComponentType)i;
	}
		
	return ComponentType::UNDEFINED;
}

Component * ComponentMgr::GetComponent(componentID & id)
{
	ComponentType compType = typeOfID(id);
	
	//id�� ��Ī�� �ȵǸ� nullptr ��ȯ
	if (idMap.find(id) == idMap.end())
		return nullptr;
	
	int idx = idMap[id];

	switch (compType)
	{
	case ComponentType::MESHRENDERER:
		return &meshRenderers[idx];
	case ComponentType::SKINNEDMESHRENDERER:
		return &skinnedMeshRenderers[idx];
	case ComponentType::LIGHT:
		return &lightings[idx];
	default:
		return components[idx];
	}
	return nullptr;
}



void ComponentMgr::Render(ID3D11DeviceContext* context, Camera* camera)
{
	for(int i=0; i<enableCount_meshRenderer; i++)
		meshRenderers[i].Draw(context, camera);
	for (int i = 0; i < enableCount_skinnedMeshRenderer; i++)
		skinnedMeshRenderers[i].Draw(context, camera);
}

void ComponentMgr::Update()
{
	for (int i = 0; i < enableCount_meshRenderer; i++)
		meshRenderers[i].Update();
	for (int i = 0; i < enableCount_skinnedMeshRenderer; i++)
		skinnedMeshRenderers[i].Update();
	for (int i = 0; i < enableCount_lighting; i++)
		lightings[i].Update();
	
	
}

