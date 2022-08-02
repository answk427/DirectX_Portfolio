#include "NodeBoneDatas.h"

void NodeHierarchy::ReserveMemory(UINT size)
{
	parentIndices.reserve(size);
	toParents.reserve(size);
	finalTransforms.reserve(size);
}



void NodeHierarchy::AddNode(std::string* parentId, std::string* childId, std::wstring* nodeName, std::shared_ptr<Transform>& transformPtr)
{
	if (childId == nullptr)
		return;
	//rootNode ������ ��� �θ� ���� -1
	if (parentId == nullptr)
		parentIndices.push_back(-1);
	else
		parentIndices.push_back(nodeIdIdx[*parentId]);
	
	nodeIdIdx[*childId] = parentIndices.size() - 1;

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());
	

	toParents.push_back(transformPtr);
	finalTransforms.push_back(identity);
}

void NodeHierarchy::DeleteNode(std::string& nodeId)
{
	auto it = nodeIdIdx.find(nodeId);
	if (it == nodeIdIdx.end())
		return;
	int idx = it->second;

	parentIndices[idx] = -2;
}

void NodeHierarchy::AddOffset(std::string & nodeId, XMFLOAT4X4 offsetMat)
{
	auto it = nodeIdIdx.find(nodeId);
	if (it == nodeIdIdx.end())
		return;
	
	int idx = it->second;
}

void NodeHierarchy::GetFinalTransform(XMMATRIX & dest, std::string & nodeId)
{
	int nodeIdx = nodeIdIdx[nodeId];
	int parentIdx = parentIndices[nodeIdx];
	std::shared_ptr<Transform> currTransform = toParents[nodeIdx].lock();
	if (!currTransform)
		return;
	
	//���� ����� �θ�ȯ ���
	dest = XMLoadFloat4x4(currTransform->GetWorld());
	
	//�θ� �ε����� -1�� ��Ʈ���, -2�� ������ ���
	while (parentIdx >= 0)
	{
		currTransform = toParents[parentIdx].lock();
		if (currTransform)
			dest = dest * XMLoadFloat4x4(currTransform->GetWorld());

		parentIdx = parentIndices[parentIdx];
	}
}

void NodeHierarchy::GetRootWorldTransform(XMMATRIX & dest)
{
	std::shared_ptr<Transform> currTransform = toParents[0].lock();
	if (!currTransform)
		return;
	//��Ʈ����� �θ�ȯ ���(������İ� ����)
	dest = XMLoadFloat4x4(currTransform->GetWorld());
}
	
//void NodeHierarchy::UpdateAnimation(float time)
//{
//	//toRoot ������Ʈ
//	m_animator->Update(time);
//	XMMATRIX toRoot;
//	XMMATRIX offset;
//	for (int i = 0; i < finalTransforms.size(); ++i)
//	{
//		toRoot = XMLoadFloat4x4(&m_animator->toRoots[i]);
//		offset = XMLoadFloat4x4(&offsets[i]);
//		XMStoreFloat4x4(&finalTransforms[i],XMMatrixMultiply(offset, toRoot));
//	}
//}
