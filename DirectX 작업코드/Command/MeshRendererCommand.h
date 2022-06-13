#pragma once

#include "MyCommand.h"

#include "Renderer.h"

enum mapType { Type_DiffuseMap, Type_NormalMap };

class SetMaterialMap : public MyCommand
{
public:
	
public:
	SetMaterialMap(MeshRenderer* renderer, UINT idx, const std::wstring& diffuseMap, mapType mt) : 
		m_meshRenderer(renderer), materialIdx(idx), textureMapName(diffuseMap),map_type(mt) {}

private:
	mapType map_type;
	MeshRenderer* m_meshRenderer;
	UINT materialIdx;
	std::wstring textureMapName;
	std::wstring beforeTextureMapName;


	// Command을(를) 통해 상속됨
	virtual void Execute() override;

	virtual void Undo() override;

};
