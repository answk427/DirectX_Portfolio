#pragma once
#include "Interface.h"
#include "GameObject.h"
#include "AssimpLoader.h"

#include <TextureMgr.h>
#include "MeshMgr.h"
#include "ComponentMgr.h"
#include "EffectMgr.h"


class ObjectMgr
{
private:
	AssimpLoader assimpLoader;
	TextureMgr& textureMgr;
	MeshMgr& meshMgr;
	ComponentMgr& componentMgr;
	EffectMgr& effectMgr;

public:
	ObjectMgr(MeshMgr& meshMgr, ComponentMgr& componentMgr, EffectMgr& effectMgr, TextureMgr& textureMgr)
		: meshMgr(meshMgr), componentMgr(componentMgr), effectMgr(effectMgr), textureMgr(textureMgr) {}
	~ObjectMgr() {}
public:
	std::vector<GameObject> gameObjects;

public:
	GameObject& CreateGameObject(std::wstring name);
	GameObject* CreateObjectFromFile(std::string fileName);
};