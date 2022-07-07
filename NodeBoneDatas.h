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

	//node id와 index 매핑
	std::map<std::string, int> nodeIdIdx;
	//node name과 index 매핑
	std::map<std::wstring, int> nodeNameIdx;
	
	void ReserveMemory(UINT size);
	void AddNode(std::string* parentId, std::string* childId, std::wstring* nodeName, std::shared_ptr<Transform>& transformPtr);
	void DeleteNode(std::string& nodeId);
	void AddOffset(std::string& nodeId, XMFLOAT4X4 offsetMat);

	std::vector<int> parentIndices;
	std::vector<XMFLOAT4X4> offsets; //뼈대로의 offset 변환
	std::vector<std::weak_ptr<Transform>> toParents; //각 노드에서 부모로의 변환
	std::vector<XMFLOAT4X4> finalTransforms; //최종변환 벡터

	void GetFinalTransform(XMMATRIX& dest, std::string& nodeName);
	void UpdateAnimation(float time);
	
};