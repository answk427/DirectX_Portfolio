#pragma once

#include <D3D11.h>

namespace Mesh
{
	//�ϳ��� �������۸� ����� ���
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