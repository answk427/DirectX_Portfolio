#pragma once

#include <map>
#include "Interface.h"
#include "GameObject.h"
#include "AssimpLoader.h"

#include <TextureMgr.h>
#include "MeshMgr.h"
#include "ComponentMgr.h"
#include "EffectMgr.h"
#include "AnimationManager.h"
#include <SkinnedData.h>
#include <random>


#define PRETEXTID "GAMEOBJECTID_"

class ObjectMgr
{
public:
	//object �����̳�
	std::map<gameObjectID, GameObject> gameObjects;
private:
	static int idNumber;
	TextureMgr& textureMgr;
	EffectMgr& effectMgr;
	MeshMgr* meshMgr;
	ComponentMgr* componentMgr;
private:
	AssimpLoader assimpLoader;
	FinalHierarchy* m_finalHierarchy;
	std::map<std::string, AssimpAnimation>* m_assimpAnimations;
	std::vector<AssimpSkinnedVertex>* m_assimpSkinnedVertices; //��, ����ġ
	BoneDatas m_boneDatas;

private:
	//Assimp���� �о�� ��带 ��������� Ž���ϴ� �Լ�
	void AddNode(GameObject* parent, NodeStruct& node);
	GameObject* AddNode(NodeStruct& node);
	Renderer* ObjectMgr::CreateRenderer(AssimpMesh& assimpMesh, const std::string& name, const gameObjectID& ownerId);
	

	//���ο� ������Ʈ�� id�� �����ϴ� �Լ�
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
	GameObject* CreateObjectFromFile(const std::string& fileName, AssimpLoader& aspLoader);
	GameObject& CreateBasicBoxObject();
	GameObject& CreateBasicBoxObject(GameObject* parent);
	GameObject& CreateBasicGrid();
	GameObject& CreateTerrain();

	GameObject* CreateLatestObject(int xRange, int yRange, int zRange);
	
	Component* AddComponent(GameObject* obj, ComponentType compType);

	bool DeleteObject(gameObjectID& id);

	void ConvertOldAnimation(const std::string& clipName, const AssimpAnimation& assimpAni, AnimationClip& dest);
};