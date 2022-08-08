#include "MeshRendererCommand.h"


void SetMaterialMap::Execute()
{
	switch (map_type)
	{
	case mapType::Type_DiffuseMap:
		beforeTextureMapName = m_Renderer->GetMaterials()[materialIdx].diffuseMapName;
		m_Renderer->ModifyDiffuseMap(materialIdx, textureMapName, mapType::Type_DiffuseMap);
		break;
	case mapType::Type_NormalMap:
		beforeTextureMapName = m_Renderer->GetMaterials()[materialIdx].normalMapName;
		m_Renderer->ModifyDiffuseMap(materialIdx, textureMapName, mapType::Type_NormalMap);
		break;
	}
}

void SetMaterialMap::Undo()
{
	m_Renderer->ModifyDiffuseMap(materialIdx, beforeTextureMapName, map_type);
}

