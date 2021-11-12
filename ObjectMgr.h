#pragma once

#include "Interface.h"
#include "GameObject.h"
#include "AssimpLoader.h"

#include <TextureMgr.h>
#include "MeshMgr.h"
#include "ComponentMgr.h"
#include "EffectMgr.h"

#include <map>

#define PRETEXTID "GAMEOBJECTID_"

class ObjectMgr
{
private:
	static int idNumber;
	AssimpLoader assimpLoader;
	TextureMgr& textureMgr;
	MeshMgr& meshMgr;
	ComponentMgr& componentMgr;
	EffectMgr& effectMgr;

private:
	//Assimp���� �о�� ��带 ��������� Ž���ϴ� �Լ�
	void AddNode(GameObject* parent, NodeStruct& node);
	GameObject* AddNode(NodeStruct& node);
	Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name);

	//���ο� ������Ʈ�� id�� �����ϴ� �Լ�
	gameObjectID makeID();
public:
	ObjectMgr(MeshMgr& meshMgr, ComponentMgr& componentMgr, EffectMgr& effectMgr, TextureMgr& textureMgr)
		: meshMgr(meshMgr), componentMgr(componentMgr), effectMgr(effectMgr), textureMgr(textureMgr) {}
	~ObjectMgr() {}
public:
	std::map<gameObjectID,GameObject> gameObjects;

public:
	GameObject& ObjectMgr::CreateGameObject(const gameObjectID& id);
	GameObject& ObjectMgr::CreateGameObject(const gameObjectID& id, const std::wstring& name);
	GameObject & ObjectMgr::CreateGameObject(const gameObjectID& id, const std::wstring & name, GameObject* parent);
	GameObject* CreateObjectFromFile(const std::string& fileName);
};