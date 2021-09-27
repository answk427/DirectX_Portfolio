#include "ComponentMgr.h"

ComponentMgr::ComponentMgr() : enableCount_meshRenderer(0), enableCount_skinnedMeshRenderer(0)
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

void ComponentMgr::Render(ID3D11DeviceContext* context, Camera* camera, CXMMATRIX world)
{
	for(int i=0; i<enableCount_meshRenderer; i++)
		meshRenderers[i].Draw(context, camera, world);
	for (int i = 0; i < enableCount_skinnedMeshRenderer; i++)
		skinnedMeshRenderers[i].Draw(context, camera, world);
}

