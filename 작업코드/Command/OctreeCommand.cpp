#include "OctreeCommand.h"

Octree* OctreeCommand::m_octree = nullptr;

void OctreeCommand::Execute()
{
}

void OctreeCommand::Undo()
{
}

void OctreeAddObject::Execute()
{
	//옥트리에 해당 오브젝트 추가
	OctreeNode* resultNode = m_octree->BuildOctree(m_Renderer, 0);
	assert(resultNode!=nullptr);
	
	//오브젝트가 어떤 노드에 속하는지 저장
	m_Renderer->m_octreeData.first = &resultNode->GetList();
	m_Renderer->m_octreeData.second = &(--resultNode->GetList().end());
	
}

void OctreeAddObject::Undo()
{

}

void OctreePopObject::Execute()
{
	auto& nodeData = m_Renderer->m_octreeData;
	if (nodeData.first == nullptr || nodeData.second == nullptr)
		return;
	//옥트리에서 해당 오브젝트 제거
	auto it = nodeData.first->begin();
	
	while(true)
	{
		if((*it)->id == m_Renderer->id)
			it = nodeData.first->erase(it);
		if (it == nodeData.first->end())
			break;
		++it;
	}
	

	nodeData.first = nullptr;
	nodeData.second = nullptr;
}

void OctreePopObject::Undo()
{
}
