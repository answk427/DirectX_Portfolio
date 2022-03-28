#pragma once
#include "BoundingBoxRenderer.h"

class SimpleLineRenderer : public BoundingBoxRenderer
{
public:
	SimpleLineRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
		:BoundingBoxRenderer(device, context) 
	{
		mesh = new Mesh("SimpleLineRenderer");
		mesh->Init(std::vector<MyVertex::BasicVertex>(), std::vector<UINT>());
		SetTransform(new Transform(this));
	}
	~SimpleLineRenderer()
	{
		if(transform!=nullptr)
			delete mesh;
	}

public:
	std::vector<MyVertex::PosColorVertex> m_vertices;
	void AddBoundingBox(const XNA::AxisAlignedBox& aabb);
	virtual void Draw(ID3D11DeviceContext * context, Camera * camera);
};
