#include "NodeBoneDatas.h"

void NodeBoneDatas::ReserveMemory(UINT size)
{
	parentIndices.reserve(size);
	toRoots.reserve(size);
	finalTransforms.reserve(size);
}

void NodeBoneDatas::AddNode(std::string* parentId, std::string* childId)
{
	if (childId == nullptr)
		return;
	//rootNode 삽입일 경우 부모 값은 -1
	if (parentId == nullptr)
		parentIndices.push_back(-1);
	else
		parentIndices.push_back(nodeNameIdx[*parentId]);
	
	nodeNameIdx[*childId] = parentIndices.size() - 1;
	

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());
	
	toRoots.push_back(identity);
	offsets.push_back(identity);
	finalTransforms.push_back(identity);
}

void NodeBoneDatas::DeleteNode(std::string * nodeId)
{
	auto it = nodeNameIdx.find(*nodeId);
	if (it == nodeNameIdx.end())
		return;
	int idx = it->second;

	parentIndices[idx] = -2;
}
