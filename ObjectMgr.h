#pragma once

#include <map>
#include "Interface.h"
#include "GameObject.h"
#include "AssimpLoader.h"

#include <TextureMgr.h>
#include "MeshMgr.h"
#include "ComponentMgr.h"
#include "EffectMgr.h"



#define PRETEXTID "GAMEOBJECTID_"

class ObjectMgr
{
public:
	//object 컨테이너
	std::map<gameObjectID, GameObject> gameObjects;
private:
	static int idNumber;
	TextureMgr& textureMgr;
	EffectMgr& effectMgr;
	MeshMgr* meshMgr;
	ComponentMgr* componentMgr;
private:
	AssimpLoader assimpLoader;
	std::map<std::string, AssimpBone>* m_assimpBones;
	std::map<std::string, AssimpAnimation>* m_assimpAnimations;
private:
	//Assimp에서 읽어온 노드를 재귀적으로 탐색하는 함수
	void AddNode(GameObject* parent, NodeStruct& node);
	GameObject* AddNode(NodeStruct& node);
	Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name);
	

	//새로운 오브젝트의 id를 생성하는 함수
	gameObjectID makeID();

public:
	ObjectMgr()
		: effectMgr(EffectMgr::Instance()), textureMgr(TextureMgr::Instance()),
		meshMgr(0), componentMgr(0)	{}
	~ObjectMgr() {}

	void Init(MeshMgr* meshMgr, ComponentMgr* componentMgr);
	static ObjectMgr& Instance();
public:
	GameObject& ObjectMgr::CreateGameObject();
	GameObject& ObjectMgr::CreateGameObject(const std::wstring& name);
	GameObject & ObjectMgr::CreateGameObject(const std::wstring & name, GameObject* parent);

	GameObject* CopyGameObject(GameObject& copyObj);
	GameObject* CopyGameObject(const std::wstring& name, GameObject& copyObj);

	GameObject* CreateObjectFromFile(const std::string& fileName);
	GameObject& CreateBasicBoxObject();
	GameObject& CreateBasicGrid();
	GameObject& CreateTerrain();
	
	Component* AddComponent(GameObject* obj, ComponentType compType);

	bool DeleteObject(gameObjectID& id);
};