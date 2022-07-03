#pragma once
#include <d3dUtil.h>
#include <vector>
#include <map>

class NodeBoneDatas
{
public:
	NodeBoneDatas() {}
	
	//node 이름과 index 매핑
	std::map<std::string, int> nodeNameIdx;
	
	void ReserveMemory(UINT size);
	void AddNode(std::string* parentId, std::string* childId);
	void DeleteNode(std::string* nodeId);

	std::vector<int> parentIndices;
	std::vector<XMFLOAT4X4> offsets; //뼈대로의 offset 변환
	std::vector<XMFLOAT4X4> toRoots; //각 뼈대에서 Root로의 변환
	std::vector<XMFLOAT4X4> finalTransforms; //최종변환 벡터
};