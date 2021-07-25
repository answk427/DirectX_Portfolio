#pragma once

#include <D3D11.h>

namespace Mesh
{
	//하나의 정점버퍼를 사용할 경우
	struct Subset
	{
		UINT VertexCount;
		UINT VertexStart;
		UINT FaceCount;
		UINT FaceStart;

		Subset() : VertexCount(0), VertexStart(0),
			FaceCount(0), FaceStart(0) {}
	};
}