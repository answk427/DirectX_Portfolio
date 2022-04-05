#include "MeshRendererCommand.h"


void SetMaterialMap::Execute()
{
	switch (map_type)
	{
	case mapType::Type_DiffuseMap:
		beforeTextureMapName = m_meshRenderer->GetMaterials()[materialIdx].diffuseMapName;
		m_meshRenderer->ModifyDiffuseMap(materialIdx, textureMapName, mapType::Type_DiffuseMap);
		break;
	case mapType::Type_NormalMap:
		beforeTextureMapName = m_meshRenderer->GetMaterials()[materialIdx].normalMapName;
		m_meshRenderer->ModifyDiffuseMap(materialIdx, textureMapName, mapType::Type_NormalMap);
		break;
	}
}

void SetMaterialMap::Undo()
{
	m_meshRenderer->ModifyDiffuseMap(materialIdx, beforeTextureMapName, map_type);
}

