#pragma once

#include <xnamath.h>

struct BasicMaterial
{
	BasicMaterial() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};

struct GeneralMaterial
{
	BasicMaterial basicMat;

	std::wstring diffuseMapName;
	std::wstring normalMapName;
	std::wstring specularMap;
	std::string ShaderName;

	//텍스쳐 행렬을 위한 변수
	XMFLOAT2 textureTiling;
	XMFLOAT2 textureOffset;

	
	GeneralMaterial() 
	{
		ZeroMemory(this, sizeof(this));
		textureTiling = { 1.0f,1.0f };
	}
};


