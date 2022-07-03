#pragma once
#include <d3dUtil.h>
#include <vector>
#include <map>

class NodeBoneDatas
{
public:
	NodeBoneDatas() {}
	
	//node �̸��� index ����
	std::map<std::string, int> nodeNameIdx;
	
	void ReserveMemory(UINT size);
	void AddNode(std::string* parentId, std::string* childId);
	void DeleteNode(std::string* nodeId);

	std::vector<int> parentIndices;
	std::vector<XMFLOAT4X4> offsets; //������� offset ��ȯ
	std::vector<XMFLOAT4X4> toRoots; //�� ���뿡�� Root���� ��ȯ
	std::vector<XMFLOAT4X4> finalTransforms; //������ȯ ����
};