#include "MeshRendererCommand.h"


void SetMaterialMap::Execute()
{
	switch (map_type)
	{
	case mapType::Type_DiffuseMap:
		beforeTextureMapName = material->diffuseMapName;
		material->diffuseMapName = textureMapName;
		break;
	case mapType::Type_NormalMap:
		beforeTextureMapName = material->normalMapName;
		material->normalMapName = textureMapName;
		break;
	}
}

void SetMaterialMap::Undo()
{
	material->diffuseMapName = beforeTextureMapName;
}

