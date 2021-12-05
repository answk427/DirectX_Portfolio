#include "ComponentMgr.h"

ComponentMgr::ComponentMgr() : enableCount_meshRenderer(0), enableCount_skinnedMeshRenderer(0),
creatingIdNum(0)
{
	//�޸� ũ�� �Ҵ�
	meshRenderers.reserve(MESHRENDERERSIZE);
	skinnedMeshRenderers.reserve(SKINNEDMESHRENDERERSIZE);
	   

	

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
			
			//id���� �� vector�� �߰�
			meshRenderers.push_back(MeshRenderer(MESHID + std::to_string(creatingIdNum++)));
			//id�� index ����
			idMap[meshRenderers.back().id] = meshRenderers.size() - 1;
			//Ȱ��ȭ ���·� �ٲ�
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



void ComponentMgr::Render(ID3D11DeviceContext* context, Camera* camera)
{
	for(int i=0; i<enableCount_meshRenderer; i++)
		meshRenderers[i].Draw(context, camera);
	for (int i = 0; i < enableCount_skinnedMeshRenderer; i++)
		skinnedMeshRenderers[i].Draw(context, camera);
}

