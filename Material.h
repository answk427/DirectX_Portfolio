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
	XMFLOAT2 textureTiling;
	XMFLOAT2 textureOffset;

	
	GeneralMaterial() { ZeroMemory(this, sizeof(this));}
};


