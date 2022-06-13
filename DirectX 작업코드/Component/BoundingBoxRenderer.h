#pragma once

#include <Renderer.h>
#include <GameObject.h>

class BoundingBoxRenderer : public Renderer 
{
public:
	BoundingBoxRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~BoundingBoxRenderer();
	void VertexUpdate();
	
private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	Mesh* m_meshOfCurrentObj;
protected:
	void PipeLineSetting();
	virtual void InitEffects();
public:
	GameObject* m_obj;
public:
	virtual void Draw(ID3D11DeviceContext * context, Camera * camera);
	void SetVB(ID3D11DeviceContext * context);
	
			
	bool SetObject(GameObject* gameObj);
	std::vector<MyVertex::PosColorVertex> AABBtoVertices(XNA::AxisAlignedBox aabb);

	
};
