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
	//��Ʈ���� �ش� ������Ʈ �߰�
	OctreeNode* resultNode = m_octree->BuildOctree(m_Renderer, 0);
	assert(resultNode!=nullptr);
	
	//������Ʈ�� � ��忡 ���ϴ��� ����
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
	//��Ʈ������ �ش� ������Ʈ ����
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
