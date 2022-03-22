#pragma once
#include "Renderer.h"

struct TreePointSprite
{
	XMFLOAT3 Pos;
	XMFLOAT2 Size;
};

class TreeBillBoardRenderer : public Renderer
{
private:
	std::vector<TreePointSprite> treeVertices;
public:
	TreeBillBoardRenderer();
	void AddTree(ID3D11Device* device, XMFLOAT3& pos, XMFLOAT2& size);

};
