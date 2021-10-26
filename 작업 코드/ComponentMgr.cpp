#include "ComponentMgr.h"

ComponentMgr::ComponentMgr() : enableCount_meshRenderer(0), enableCount_skinnedMeshRenderer(0),
creatingIdNum(0)
{
	typeMap[typeid(MeshRenderer).raw_name()] = ComponentType::MESHRENDERER;
	typeMap[typeid(SkinnedMeshRenderer).raw_name()] = ComponentType::SKINNEDMESHRENDERER;

}


Component * ComponentMgr::OnOffComponent(Component * component, Command endisable)
{
	ComponentType type = GetComponentType(component);
			
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
		default:
			return component;
	}
}


Component * ComponentMgr::CreateComponent(ComponentType compType)
{
	switch (compType)
	{
		case ComponentType::MESHRENDERER :
		{
			
			//id생성 후 vector에 추가
			meshRenderers.push_back(MeshRenderer(MESHID + std::to_string(creatingIdNum++)));
			//id와 index 매핑
			idMap[meshRenderers.back().id] = meshRenderers.size() - 1;
			//활성화 상태로 바꿈
			return SelectSwap(meshRenderers, enableCount_meshRenderer,
				meshRenderers.size() - 1, Command::ENABLE);

			
		}
		case ComponentType::SKINNEDMESHRENDERER :
		{
			skinnedMeshRenderers.push_back(
				SkinnedMeshRenderer(SKINNEDMESHID + std::to_string(creatingIdNum++)));
			idMap[skinnedMeshRenderers.back().id] = skinnedMeshRenderers.size() - 1;
			return SelectSwap(skinnedMeshRenderers, enableCount_skinnedMeshRenderer,
				skinnedMeshRenderers.size() - 1, Command::ENABLE);
		}
			
	}
	return nullptr;
}

ComponentType ComponentMgr::GetComponentType(Component * component)
{
	std::string typeName = typeid(*component).raw_name();

	std::unordered_map<std::string, ComponentType>::iterator it
		= typeMap.find(typeName);

	//알수없는 타입인지 확인
	assert(it != typeMap.end());

	ComponentType type = (*it).second;

	return type;
}

void ComponentMgr::Render(ID3D11DeviceContext* context, Camera* camera)
{
	for(int i=0; i<enableCount_meshRenderer; i++)
		meshRenderers[i].Draw(context, camera);
	for (int i = 0; i < enableCount_skinnedMeshRenderer; i++)
		skinnedMeshRenderers[i].Draw(context, camera);
}

