#include "NodeBoneDatas.h"

void NodeBoneDatas::ReserveMemory(UINT size)
{
	parentIndices.reserve(size);
	toParents.reserve(size);
	finalTransforms.reserve(size);
}



void NodeBoneDatas::AddNode(std::string* parentId, std::string* childId, std::wstring* nodeName, std::shared_ptr<Transform>& transformPtr)
{
	if (childId == nullptr)
		return;
	//rootNode 삽입일 경우 부모 값은 -1
	if (parentId == nullptr)
		parentIndices.push_back(-1);
	else
		parentIndices.push_back(nodeIdIdx[*parentId]);
	
	nodeIdIdx[*childId] = parentIndices.size() - 1;
	nodeNameIdx[*nodeName] = parentIndices.size() - 1;

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());
	

	toParents.push_back(transformPtr);
	offsets.push_back(identity);
	finalTransforms.push_back(identity);
}

void NodeBoneDatas::DeleteNode(std::string& nodeId)
{
	auto it = nodeIdIdx.find(nodeId);
	if (it == nodeIdIdx.end())
		return;
	int idx = it->second;

	parentIndices[idx] = -2;
}

void NodeBoneDatas::AddOffset(std::string & nodeId, XMFLOAT4X4 offsetMat)
{
	auto it = nodeIdIdx.find(nodeId);
	if (it == nodeIdIdx.end())
		return;
	
	int idx = it->second;

	offsets[idx] = offsetMat;
}

void NodeBoneDatas::GetFinalTransform(XMMATRIX & dest, std::string & nodeId)
{
	int nodeIdx = nodeIdIdx[nodeId];
	int parentIdx = parentIndices[nodeIdx];
	std::shared_ptr<Transform> currTransform = toParents[nodeIdx].lock();
	if (!currTransform)
		return;
	
	//현재 노드의 부모변환 행렬
	dest = XMLoadFloat4x4(currTransform->GetWorld());
	
	//부모 인덱스가 -1은 루트노드, -2는 삭제된 노드
	while (parentIdx >= 0)
	{
		currTransform = toParents[parentIdx].lock();
		if (currTransform)
			dest = dest * XMLoadFloat4x4(currTransform->GetWorld());

		parentIdx = parentIndices[parentIdx];
	}
}

void NodeBoneDatas::UpdateAnimation(float time)
{
	//toRoot 업데이트
	m_animator->Update(time);
	XMMATRIX toRoot;
	XMMATRIX offset;
	for (int i = 0; i < finalTransforms.size(); ++i)
	{
		toRoot = XMLoadFloat4x4(&m_animator->toRoots[i]);
		offset = XMLoadFloat4x4(&offsets[i]);
		XMStoreFloat4x4(&finalTransforms[i],XMMatrixMultiply(offset, toRoot));
	}
}
