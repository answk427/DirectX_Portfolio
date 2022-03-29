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
	~TreeBillBoardRenderer();
	void AddTree(ID3D11Device* device, XMFLOAT3& pos, XMFLOAT2& size);
	void DeleteTree(ID3D11Device* device, UINT treeIdx);

public:
	virtual void InitDiffuseMaps();
	virtual void InitEffects();
	virtual void SetVB(ID3D11DeviceContext* context);
	
public:
	std::vector<std::wstring> treeTextureNames;

};
