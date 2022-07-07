#pragma once
#include <d3dUtil.h>
#include <vector>
#include <map>
#include "Transform.h"
#include "AnimationClass.h"

class NodeBoneDatas
{
public:
	NodeBoneDatas() : m_animator(std::make_unique<Animator>(nodeNameIdx,parentIndices))
		{}
	
	std::unique_ptr<Animator> m_animator;

	//node id�� index ����
	std::map<std::string, int> nodeIdIdx;
	//node name�� index ����
	std::map<std::wstring, int> nodeNameIdx;
	
	void ReserveMemory(UINT size);
	void AddNode(std::string* parentId, std::string* childId, std::wstring* nodeName, std::shared_ptr<Transform>& transformPtr);
	void DeleteNode(std::string& nodeId);
	void AddOffset(std::string& nodeId, XMFLOAT4X4 offsetMat);

	std::vector<int> parentIndices;
	std::vector<XMFLOAT4X4> offsets; //������� offset ��ȯ
	std::vector<std::weak_ptr<Transform>> toParents; //�� ��忡�� �θ���� ��ȯ
	std::vector<XMFLOAT4X4> finalTransforms; //������ȯ ����

	void GetFinalTransform(XMMATRIX& dest, std::string& nodeName);
	void UpdateAnimation(float time);
	
};