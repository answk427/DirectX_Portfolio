#pragma once
#include <d3dUtil.h>
#include <vector>
#include <map>
#include "Transform.h"
#include "AnimationClass.h"
#include "Effects.h"

class NodeHierarchy
{
public:
	UINT mTechType;
public:
	NodeHierarchy() : m_animator(std::make_shared<Animator>()),
		mTechType(TechniqueType::Light)
		{}
	
	std::shared_ptr<Animator> m_animator;

	//node id와 index 매핑
	std::map<std::string, int> nodeIdIdx;
	
	void ReserveMemory(UINT size);
	void AddNode(std::string* parentId, std::string* childId, std::wstring* nodeName, std::shared_ptr<Transform>& transformPtr);
	void DeleteNode(std::string& nodeId);
	void AddOffset(std::string& nodeId, XMFLOAT4X4 offsetMat);

	std::vector<int> parentIndices;
	std::vector<std::weak_ptr<Transform>> toParents; //각 노드에서 부모로의 변환
	std::vector<XMFLOAT4X4> finalTransforms; //최종변환 벡터

	void GetFinalTransform(XMMATRIX& dest, std::string& nodeName);
	void GetRootWorldTransform(XMMATRIX & dest);
	//void UpdateAnimation(float time);
	
};