//***************************************************************************************
// LightHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper classes for lighting.
//***************************************************************************************

#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <Windows.h>
#include <xnamath.h>

// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.

struct DirectionalLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
	
	DirectionalLight() : Ambient(1,1,1,1), Diffuse(1,1,1,1), Specular(1,1,1,1), Direction(0,0,1), Pad(0)
	{ //ZeroMemory(this, sizeof(*this)); 
	}
};

struct PointLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.

	PointLight() : Ambient(1, 1, 1, 1), Diffuse(1, 1, 1, 1), Specular(1, 1, 1, 1),
		Position(0, 0, 0), Range(0), Att(1,1,1),Pad(0)
	{// ZeroMemory(this, sizeof(*this)); 
	}
};


struct SpotLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	// Packed into 4D vector: (Direction, Spot)
	XMFLOAT3 Direction;
	float Spot;
	
	// Packed into 4D vector: (Att, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
	
	SpotLight() :Ambient(1, 1, 1, 1), Diffuse(1, 1, 1, 1), Specular(1, 1, 1, 1),
		Position(0, 0, 0), Range(0), Direction(0,0,1), Spot(1.0f), Att(1,1,1), Pad(0)
	{// ZeroMemory(this, sizeof(*this));
	}
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(*this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};

#endif // LIGHTHELPER_H