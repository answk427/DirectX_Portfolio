#pragma once

#include <xnamath.h>

struct BasicMaterial
{
	BasicMaterial() : Ambient(1.0f,1.0f,1.0f,1.0f), Diffuse(1.0f,1.0f,1.0f,1.0f),
		Specular(1.0f,1.0f,1.0f,1.0f), Reflect(1.0f,1.0f,1.0f,1.0f)
	{ }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};

struct GeneralMaterial
{
	std::wstring name;
	BasicMaterial basicMat;

	std::wstring diffuseMapName;
	std::wstring normalMapName;
	std::wstring specularMap;
	std::wstring ShaderName;

	//�ؽ��� ����� ���� ����
	XMFLOAT2 textureTiling;
	XMFLOAT2 textureOffset;

	//���׸��� ���� �����ϴ� ����
	XMFLOAT4 m_color;
	

	GeneralMaterial() 
	{
		ZeroMemory(this, sizeof(this));
		textureTiling = { 1.0f,1.0f };
		textureOffset = { 0.0f, 0.0f };
		diffuseMapName = L"StandardDiffuseMap";
		m_color = { 1.0f,1.0f,1.0f,1.0f };
		name = L"Material";
	}
};


