#include "ComponentMgr.h"

std::vector<std::string> idFront = { "MESHRENDERER", "SKINNEDMESHRENDERER" , "LIGHT", "DEFAULT" };

ComponentMgr::ComponentMgr() : enableCount_meshRenderer(0), enableCount_skinnedMeshRenderer(0)
, enableCount_lighting(0),creatingIdNum(0)
{
	//메모리 크기 할당
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
			
			//id생성 후 vector에 추가
			meshRenderers.push_back(MeshRenderer(id));
			//id와 index 매핑
			idMap[meshRenderers.back().id] = meshRenderers.size() - 1;
			//활성화 상태로 바꿈
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
	
	//ID와 일치하는 컴포넌트타입을 검사 후 반환
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
	
	//id와 매칭이 안되면 nullptr 반환
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

const std::vector<Renderer*> ComponentMgr::GetDrawableRenderers()
{
	std::vector<Renderer*> result;

	for (int i = 0; i < enableCount_meshRenderer; ++i)
		result.push_back(&meshRenderers[i]);
	for (int i = 0; i < enableCount_skinnedMeshRenderer; ++i)
		result.push_back(&skinnedMeshRenderers[i]);
	
	return result;
}



void ComponentMgr::Render(ID3D11DeviceContext* context, Camera* camera)
{
	//혼합 객체를 나중에 그리기 위해 index를 모아두는 vector
	static std::vector<std::pair<float,int>> blendMeshRenderers;
	static std::vector<std::pair<float,int>> blendSkinnedMeshRenderers;

	XMVECTOR cameraPos = XMLoadFloat3(&camera->GetPosition());
	//비혼합 객체 렌더링
	for (int i = 0; i < enableCount_meshRenderer; i++)
	{
		if (meshRenderers[i].GetBlending())
		{
			//카메라와 해당 객체 사이의 거리를 구함
			XMVECTOR dist = XMVector3LengthEst(cameraPos - XMLoadFloat3(&meshRenderers[i].GetTransform()->m_position));
			blendMeshRenderers.push_back({ XMVectorGetX(dist), i });
		}
		else
			meshRenderers[i].Draw(context, camera);
		
	}
	for (int i = 0; i < enableCount_skinnedMeshRenderer; i++)
	{
		if (skinnedMeshRenderers[i].GetBlending())
		{
			//카메라와 해당 객체 사이의 거리를 구함
			XMVECTOR dist = XMVector3LengthEst(cameraPos - XMLoadFloat3(&skinnedMeshRenderers[i].GetTransform()->m_position));
			blendSkinnedMeshRenderers.push_back({ XMVectorGetX(dist), i });
		}
		else
			skinnedMeshRenderers[i].Draw(context, camera);
	}
		
	//혼합객체 거리기준 정렬 후 렌더링
	if (!blendMeshRenderers.empty())
	{
		std::sort(blendMeshRenderers.begin(), blendMeshRenderers.end(), std::greater<pair<float, int>>());
		for(auto elem : blendMeshRenderers)
			meshRenderers[elem.second].Draw(context, camera);
		blendMeshRenderers.clear();
	}
	if (!blendSkinnedMeshRenderers.empty())
	{
		std::sort(blendSkinnedMeshRenderers.begin(), blendSkinnedMeshRenderers.end(), std::greater<pair<float,int>>());
		for (auto elem : blendSkinnedMeshRenderers)
			skinnedMeshRenderers[elem.second].Draw(context, camera);
		blendSkinnedMeshRenderers.clear();
	}
		
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

