#pragma once

#include "MyCommand.h"

#include "Renderer.h"

enum mapType { Type_DiffuseMap, Type_NormalMap };

class SetMaterialMap : public MyCommand
{
public:
	
public:
	SetMaterialMap(GeneralMaterial* mat, const std::wstring& diffuseMap, mapType mt) : material(mat), textureMapName(diffuseMap),map_type(mt) {}

private:
	mapType map_type;
	GeneralMaterial* material;
	std::wstring textureMapName;
	std::wstring beforeTextureMapName;


	// Command을(를) 통해 상속됨
	virtual void Execute() override;

	virtual void Undo() override;

};
