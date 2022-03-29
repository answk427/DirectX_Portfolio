#pragma once
#include "Octree.h"
#include "MyCommand.h"
#include "SimpleLineRenderer.h"

class OctreeCommand : public MyCommand
{
protected:
	static Octree* m_octree;
	
public:
	static void Init(Octree* octree) { m_octree = octree; }

	// MyCommand을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
};

class OctreeAddObject : public OctreeCommand
{
private:
	Renderer* m_Renderer;
public:
	OctreeAddObject(Renderer* renderer) : m_Renderer(renderer) {}

	// MyCommand을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
};

class OctreePopObject : public OctreeCommand
{
private:
	Renderer* m_Renderer;
public:
	OctreePopObject(Renderer* renderer) : m_Renderer(renderer) {}

	// MyCommand을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
};
