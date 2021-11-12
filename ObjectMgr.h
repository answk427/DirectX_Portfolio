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
	//Assimp에서 읽어온 노드를 재귀적으로 탐색하는 함수
	void AddNode(GameObject* parent, NodeStruct& node);
	GameObject* AddNode(NodeStruct& node);
	Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name);

	//새로운 오브젝트의 id를 생성하는 함수
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