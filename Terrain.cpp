#include "Terrain.h"

std::vector<int> TerrainRenderer::HeightMapSizes = { 33,65,129,257,513,1025,2049 };

void TerrainRenderer::InitEffects()
{
	EffectMgr::Instance().SetType(L"FX/Terrain.fxo", EffectType::Terrain);
	effects.clear();
	for (auto& elem : materials)
	{
		Effect* effect = m_effectMgr.GetEffect(L"FX/Terrain.fxo");
		effects.push_back(effect);
	}
}

void TerrainRenderer::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (mesh == nullptr)
		return;

	XMMATRIX world = XMLoadFloat4x4(&transform->m_world);


	//��������, �ε������۸� �Է������⿡ ����
	//if (GetInstancing())
	//{
	//	//�̹� �������� �߰ų�, �׸� ������Ʈ���� ���� ��� ����
	//	if (mesh->enableInstancingIndexes.empty())
	//		return;
	//	mesh->InstancingUpdate(context);
	//	mesh->SetInstanceVB(context);
	//}
	//else
	//	SetVB(context);
	UINT stride[1] = { sizeof(MyVertex::Terrain) };
	UINT offset[1] = { 0 };
	ID3D11Buffer* vb = mesh->GetVB();
	context->IASetVertexBuffers(0, 1, &vb, stride, offset);


	mesh->SetIB(context);

	//����->���� ��ķκ��� ����ü�� ����.
	XMMATRIX viewProj = camera->ViewProj();
	XMFLOAT4 worldPlanes[6];
	ExtractFrustumPlanes(worldPlanes, viewProj);

	ID3DX11EffectTechnique* activeTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	UINT tempTechType = m_technique_type;

	if (isShadowMapRender())
		tempTechType = tempTechType | TechniqueType::Tesselation;

	effects[0]->IASetting(context, tempTechType);
	effects[0]->OMSetting(context, m_blending);



	if (isShadowRender())
		tempTechType = tempTechType | TechniqueType::Shadowed;

	if (m_enableFog)
		tempTechType = tempTechType | TechniqueType::Fog;


	//if (GetInstancing())
	//	activeTech = terrainEffect->GetTechnique(tempTechType | TechniqueType::Instancing);
	//else



	//shader�� �ʿ��� ������ ����
	effects[0]->PerObjectSet(&materials[0],
		camera, world);

	//�Ϲ� �������� ��
	if (!isRenderShadowMapBaking)
	{
		TerrainEffect* terrainEffect = static_cast<TerrainEffect*>(effects[0]);

		terrainEffect->SetMinDist(m_tesselationData.minDist);
		terrainEffect->SetMaxDist(m_tesselationData.maxDist);
		terrainEffect->SetMinTess(m_tesselationData.minTess);
		terrainEffect->SetMaxTess(m_tesselationData.maxTess);
		terrainEffect->SetHeightMap(mHeightMapSRV);

		// Set per frame constants.
		terrainEffect->SetFogColor(Colors::Silver);
		terrainEffect->SetFogStart(m_fogStart);
		terrainEffect->SetFogRange(m_fogRange);


		terrainEffect->SetTexelCellSpaceU(1.0f / m_terrainData.HeightmapWidth);
		terrainEffect->SetTexelCellSpaceV(1.0f / m_terrainData.HeightmapHeight);
		terrainEffect->SetWorldCellSpace(m_terrainData.CellSpacing);
		terrainEffect->SetWorldFrustumPlanes(worldPlanes);

		terrainEffect->SetLayerMapArray(mtextureArraySRV);
		terrainEffect->SetBlendMap(mblendTextureSRV);

		terrainEffect->SetBrush(*m_brush.get());
	}
	else if (isShadowMapRender())
	{
		BuildShadowMapEffect* shadowMapEffect = static_cast<BuildShadowMapEffect*>(effects[0]);
		shadowMapEffect->SetMinTessDistance(m_tesselationData.minDist);
		shadowMapEffect->SetMaxTessDistance(m_tesselationData.maxDist);
		shadowMapEffect->SetMinTessFactor(m_tesselationData.minTess);
		shadowMapEffect->SetMaxTessFactor(m_tesselationData.maxTess);
		shadowMapEffect->SetHeightMap(mHeightMapSRV);
	}


	//�ν��Ͻ��� �� �ؽ��Ĺ迭 ���
	/*if (GetInstancing())
		terrainEffect->SetMapArray(mesh->textureArrays[i]);
	else
	{

	}*/


	activeTech = effects[0]->GetTechnique(tempTechType);
	activeTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		activeTech->GetPassByIndex(p)->Apply(0, context);

		context->DrawIndexed(mNumPatchQuadFaces * 4, 0, 0);
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	context->HSSetShader(0, 0, 0);
	context->DSSetShader(0, 0, 0);

	//instancing �������� Draw ȣ�� �ѹ��� �ϵ��� ���͸� �����.
	/*if (GetInstancing())
		mesh->enableInstancingIndexes.clear();*/

		//���� �������� �׸��ڸ� �������̸�
	if (isShadowMapRender())
		isRenderShadowMapBaking = false;

}

void TerrainRenderer::Init()
{
	// Divide heightmap into patches such that each patch has CellsPerPatch.
	mNumPatchVertRows = ((m_terrainData.HeightmapHeight - 1) / CellsPerPatch) + 1;
	mNumPatchVertCols = ((m_terrainData.HeightmapWidth - 1) / CellsPerPatch) + 1;

	mNumPatchVertices = mNumPatchVertRows * mNumPatchVertCols;
	mNumPatchQuadFaces = (mNumPatchVertRows - 1)*(mNumPatchVertCols - 1);

	LoadHeightmap();
	Smooth();
	CalcAllPatchBoundsY();

	ID3D11Device* device = m_texMgr.md3dDevice;
	BuildQuadPatchVB(device);
	BuildQuadPatchIB(device);
	BuildHeightmapSRV(device);
	BuildLayermapSRV();
	BuildBlendmapSRV();
}

void TerrainRenderer::Update()
{
	GetScreenIntersect();


	if (GetAsyncKeyState(VK_LBUTTON))
	{
		switch (m_modifyMode)
		{
		case 1:
			RaiseHeight(m_brush->centerW.x, m_brush->centerW.z);
			break;
		case 2:
			ModifyBlendMap(m_brush->centerW.x, m_brush->centerW.z);
			break;
		}



	}
	//Effects::DebugTexFX->SetTexture(mHeightMapSRV);
	//Effects::DebugTexFX->SetTexture(mblendTextureSRV);
}

float TerrainRenderer::GetWidth()const
{
	// Total terrain width.
	return (m_terrainData.HeightmapWidth - 1)*m_terrainData.CellSpacing;
}

float TerrainRenderer::GetDepth()const
{
	// Total terrain depth.
	return (m_terrainData.HeightmapHeight - 1)*m_terrainData.CellSpacing;
}

float TerrainRenderer::GetHeight(float x, float z)const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*GetWidth()) / m_terrainData.CellSpacing;
	float d = (z - 0.5f*GetDepth()) / -m_terrainData.CellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = mHeightmap[row*m_terrainData.HeightmapWidth + col];
	float B = mHeightmap[row*m_terrainData.HeightmapWidth + col + 1];
	float C = mHeightmap[(row + 1)*m_terrainData.HeightmapWidth + col];
	float D = mHeightmap[(row + 1)*m_terrainData.HeightmapWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

float TerrainRenderer::GetTangent(float posX, float posZ, float x, float z)const
{
	if (x == 0 && z == 0)
		return 0.0f;
	// Transform from terrain local space to "cell" space.
	float c = (posX + 0.5f*GetWidth()) / m_terrainData.CellSpacing;
	float d = (posZ - 0.5f*GetDepth()) / -m_terrainData.CellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = mHeightmap[row*m_terrainData.HeightmapWidth + col];
	float B = mHeightmap[row*m_terrainData.HeightmapWidth + col + 1];
	float C = mHeightmap[(row + 1)*m_terrainData.HeightmapWidth + col];
	float D = mHeightmap[(row + 1)*m_terrainData.HeightmapWidth + col + 1];

	XMVECTOR dir = XMVectorSet(x, 0.0f, z, 0.0f); //���⺤��
	XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	float dotResult = XMVectorGetX(XMVector3Dot(dir, forward)); //�� ������ ���� = cos��Ÿ ��

	float theta = XMConvertToDegrees(acosf(dotResult)); //���������� ��Ÿ�� ���


	if (x < 0) theta = 360.0f - theta; // �������� ���̰��� 30����, 330��.
	int idx = theta / 45; //45�� ���� 8���� ������ �Ѱ��� ����
	float offset = (theta - 45 * idx) / 45;

	int idxResult[8] =
	{
		row * m_terrainData.HeightmapWidth + col,
		row * m_terrainData.HeightmapWidth + col + 1,
		(row + 1) * m_terrainData.HeightmapWidth + col + 1,
		(row + 2) * m_terrainData.HeightmapWidth + col + 1,
		(row + 2) * m_terrainData.HeightmapWidth + col,
		(row + 2) * m_terrainData.HeightmapWidth + col - 1,
		(row + 1) * m_terrainData.HeightmapWidth + col - 1,
		row * m_terrainData.HeightmapWidth + col - 1
	};

	float secondHeight = offset * mHeightmap[idxResult[(idx + 1) % 8]] +
		(1 - offset) * mHeightmap[idxResult[idx]];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	float firstHeight = 0;
	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		firstHeight = A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		firstHeight = D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
	float tan = (secondHeight - firstHeight) / sqrtf(2.0f) *m_terrainData.CellSpacing;
	return abs(atanf(tan));
}

bool TerrainRenderer::GetLocalPosition(float posX, float posZ, float * destX, float * destZ)
{
	bool result = true;
	// Transform from terrain local space to "cell" space.
	*destX = (posX + 0.5f*GetWidth()) / m_terrainData.CellSpacing;
	*destZ = (posZ - 0.5f*GetDepth()) / -m_terrainData.CellSpacing;

	if (*destX < 0)
	{
		*destX = 0;
		result = false;
	}

	if (*destX >= m_terrainData.HeightmapWidth)
	{
		*destX = m_terrainData.HeightmapWidth - 1;
		result = false;
	}

	if (*destZ < 0)
	{
		*destZ = 0;
		result = false;
	}

	if (*destZ >= m_terrainData.HeightmapHeight)
	{
		*destZ = m_terrainData.HeightmapHeight - 1;
		result = false;
	}


	return result;
}

bool TerrainRenderer::GetRayIntersectPos(XMVECTOR & rayOrigin, XMVECTOR & rayDir, XMVECTOR* destPos)
{
	float x, y, z;
	float halfWidth = GetWidth() * 0.5f;
	float halfDepth = GetDepth() * 0.5f;
	XMVECTOR leftTop, rightTop, leftBottom, rightBottom;

	int patchIdxRow = -1;
	int patchIdxCol = -1;
	float distMax = MathHelper::Infinity;
	float dist;
	int aabbSize = m_aabb.size();
	const XNA::AxisAlignedBox* aabbArr = m_aabb.data();
	for (int i = 0; i < aabbSize; ++i)
	{
		if (XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &aabbArr[i], &dist))
		{
			//�ش� ��ġ�� ��� �� �ε����� ����.
			if (dist < distMax)
			{
				distMax = dist;
				patchIdxRow = i / (mNumPatchVertCols - 1);
				patchIdxCol = i % (mNumPatchVertCols - 1);
			}
		}
	}
	//�����ϴ� aabb�� �����Ƿ� false ��ȯ
	if (patchIdxRow == -1 && patchIdxCol == -1)
		return false;

	const float* heightArr = mHeightmap.data();
	//�����ϴ� aabb �ȿ� �ִ� �ﰢ���鿡 ���Ͽ� �˻�
	for (int i = patchIdxRow * CellsPerPatch; i < (patchIdxRow + 1)*CellsPerPatch - 1; ++i)
	{
		for (int j = patchIdxCol * CellsPerPatch; j < (patchIdxCol + 1)*CellsPerPatch - 1; ++j)
		{
			//���� ��
			//���� ��ǥ�� ��ȯ
			LocalToWorld(j, i, &x, &z, halfWidth, halfDepth);
			//y = mHeightmap[i*m_terrainData.HeightmapWidth + j];
			y = heightArr[i*m_terrainData.HeightmapWidth + j];
			leftTop = XMVectorSet(x, y, z, 1.0f);

			//������ �� ����
			LocalToWorld(j + 1, i, &x, &z, halfWidth, halfDepth);
			//y = mHeightmap[i*m_terrainData.HeightmapWidth + (j + 1)];
			y = heightArr[i*m_terrainData.HeightmapWidth + (j + 1)];
			rightTop = XMVectorSet(x, y, z, 1.0f);

			//���� �Ʒ� ����
			LocalToWorld(j, i + 1, &x, &z, halfWidth, halfDepth);
			//y = mHeightmap[(i + 1)*m_terrainData.HeightmapWidth + j];
			y = heightArr[(i + 1)*m_terrainData.HeightmapWidth + j];
			leftBottom = XMVectorSet(x, y, z, 1.0f);

			//������ �Ʒ� ����
			LocalToWorld(j + 1, i + 1, &x, &z, halfWidth, halfDepth);
			//y = mHeightmap[(i + 1)*m_terrainData.HeightmapWidth + (j + 1)];
			y = heightArr[(i + 1)*m_terrainData.HeightmapWidth + (j + 1)];
			rightBottom = XMVectorSet(x, y, z, 1.0f);

			//���� �ﰢ��, ������ �ﰢ���� ���� �ϴ��� �˻�
			if (XNA::IntersectRayTriangle(rayOrigin, rayDir, leftTop, rightTop, leftBottom, &dist) ||
				XNA::IntersectRayTriangle(rayOrigin, rayDir, rightBottom, rightTop, leftBottom, &dist))
			{
				*destPos = rayOrigin + dist * rayDir;
				return true;
			}
		}
	}
	//���� �ϴ°� ������ false ��ȯ
	return false;
}

bool TerrainRenderer::GetScreenIntersect()
{
	if (!m_camera || m_brush->shape == BrushShape::NOBRUSH)
		return false;
	D3D11_VIEWPORT* viewPort = Mouse::GetViewPort();
	if (viewPort == nullptr)
		return false;


	//���콺 ��ġ�� ��ũ�� ��ǥ���� ������� ������ ���

	std::pair<XMVECTOR, XMVECTOR> ray; //<����������,����������>

	float sx, sy;
	Mouse::GetScreenPos(&sx, &sy);

	RayPicking::ScreenToViewRay(&ray.first, &ray.second,
		sx, sy, viewPort, &m_camera->Proj());

	ray = RayPicking::ViewToWorldRay(&ray.first, &ray.second, &m_camera->View());


	XMVECTOR intersectionPos;
	if (!GetRayIntersectPos(ray.first, ray.second, &intersectionPos))
	{
		m_brush->padding[0] = -1.0f;
		return false;
	}

	m_brush->padding[0] = 1.0f;
	XMStoreFloat3(&m_brush->centerW, intersectionPos);

	//std::wstring str = L"x: " + std::to_wstring(m_brush->center.x) + L" y: " + std::to_wstring(m_brush->center.y) + L" z: " + std::to_wstring(m_brush->center.z) + L'\n';
	//OutputDebugString(str.c_str());

	return true;
}

void TerrainRenderer::LocalToWorld(float x, float z, float* destX, float* destZ, float halfWidth, float halfDepth)
{
	*destX = x * m_terrainData.CellSpacing - halfWidth;
	*destZ = z * -m_terrainData.CellSpacing + halfDepth;
}

void TerrainRenderer::RaiseHeight(float x, float z)
{
	if (m_brush->shape == BrushShape::NOBRUSH || m_brush->padding[0] < 0)
		return;

	//������ �ڿ����� ����� heightMap �ؽ��ĸ� ����.

	D3D11_MAPPED_SUBRESOURCE mappedData;
	//HR(m_texMgr.m_context->Map(m_hmapTex, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//HALF* heightMapData = reinterpret_cast<HALF*>(mappedData.pData);
	D3D11_TEXTURE2D_DESC heightmapDesc;
	m_hmapTex->GetDesc(&heightmapDesc);

	static UINT widthTex = 0;
	static UINT heightTex = 0;
	static std::vector<float> tempHeightMap;

	UINT width = heightmapDesc.Width;
	UINT height = heightmapDesc.Height;

	if (widthTex != width || heightTex != height)
	{
		widthTex = heightmapDesc.Width;
		heightTex = heightmapDesc.Height;
		tempHeightMap.clear();
		tempHeightMap.resize(widthTex * heightTex, 0.0f);
	}


	

	//�귯���� �߽ɿ��� ���簢�� ������ �˻�
	float leftTopX, leftTopZ;
	float rightBottomX, rightBottomZ;
	//����������� radius ������ŭ �̵��� ��ǥ -> heightmap�� ���Ǵ� ��ǥ
	GetLocalPosition(x - m_brush->radius, z + m_brush->radius, &leftTopX, &leftTopZ);
	GetLocalPosition(x + m_brush->radius, z - m_brush->radius, &rightBottomX, &rightBottomZ);

	float radiusTex = (rightBottomX - leftTopX) * 0.5f;
	float centerTexRow = leftTopZ + radiusTex;
	float centerTexCol = leftTopX + radiusTex;

	UINT idx;
	std::vector<std::tuple<UINT, UINT, UINT>> indices; //��Ȱȭ�Ҷ� ������ �ε���(row,col,idx)

	

	//float addVal = 0.0f;
	//for (int i = leftTopZ; i <= rightBottomZ; ++i)
	//{
	//	for (int j = leftTopX; j <= rightBottomX; ++j)
	//	{
	//		idx = i * width + j;

	//		//�߽ɰ��� �Ÿ�
	//		float dist = sqrtf(pow((centerTexRow - i), 2) + pow((centerTexCol - j), 2));
	//		if (dist > radiusTex)
	//			continue;

	//		indices.push_back({ i,j,idx });
	//		//�Ÿ��� ���� 2���Լ� ������� ���̸� ������ų ����� ����(0~1)
	//		yCoefficient = -(dist*dist) / (radiusTex*radiusTex) + 1;
	//		addVal = m_terrainData.HeightScale * RAISEDELTA * yCoefficient;
	//		if (m_modifyMapOption == 0)
	//		{
	//			mHeightmap[idx] = min(m_terrainData.HeightScale,
	//				mHeightmap[idx] + addVal);
	//		}
	//		else
	//		{
	//			mHeightmap[idx] = max(0,
	//				mHeightmap[idx] - addVal);
	//		}
	//		//�ִ� ���̸� ������ ���� ��	

	//		//heightMapData[idx] = XMConvertFloatToHalf(mHeightmap[idx]);
	//	}
	//}

	//float yCoefficient = 0.0f;

	//for (UINT i = 0; i < boxHeight; ++i)
	//{
	//	//4���� ���Ҹ� �����ϸ� R8G8B8A8_UNORM�̹Ƿ� ���ȼ� �����ϴ°��� ��.
	//	//col�� �����ϴ� �迭 �ε����� ���ٿ� width*4
	//	for (UINT j = 0; j < boxWidth; ++j)
	//	{
	//		UINT texI = updateBox.top + i;
	//		UINT texJ = updateBox.left + j;
	//		//����ʿ����� �ε��� ���
	//		idx = texI* width + texJ;
	//		
	//		//�귯���� ������ ���� ���� �Ÿ�
	//		float dist = sqrtf(pow((centerTexRow - texI), 2) + pow((centerTexCol - texJ), 2));
	//		indices.push_back({ texI,texJ,idx });


	//		//�Ÿ��� ���� 2���Լ� ������� ���̸� ������ų ����� ����(0~1)
	//		yCoefficient = -(dist*dist) / (radiusTex*radiusTex) + 1;
	//		
	//		addVal = m_terrainData.HeightScale * RAISEDELTA;
	//		if(m_brush->shape == BrushShape::CIRCLE)
	//			addVal = addVal * yCoefficient;
	//		
	//			

	//					
	//		//���� �귯���� �� �� ���� ���� ������Ʈ���� ����
	//		if (m_brush->shape == BrushShape::CIRCLE &&
	//			dist > radiusTex)
	//		{
	//			
	//		}
	//		else
	//		{
	//			if (m_modifyMapOption == 0)
	//			{
	//				tempHeightMap[idx] = min(m_terrainData.HeightScale,
	//					mHeightmap[idx] + addVal);
	//			}
	//			else
	//			{
	//				mHeightmap[idx] = max(0,
	//					mHeightmap[idx] - addVal);
	//			}
	//		}

	//		srcTemp[i * width + j] = tempBlendMap[idx];
	//	}
	//}

	D3D11_BOX updateBox;
	updateBox.top = leftTopZ;
	updateBox.left = leftTopX;
	updateBox.bottom = rightBottomZ;
	updateBox.right = rightBottomX;
	updateBox.front = 0;
	updateBox.back = 1;

	UINT boxWidth = updateBox.right - updateBox.left;
	UINT boxHeight = updateBox.bottom - updateBox.top;

	float addVal = 0.0f;
	switch (m_brush->shape)
	{
	case BrushShape::SQUARE:
		for (int i = leftTopZ; i < updateBox.bottom; ++i)
		{
			for (int j = leftTopX; j < updateBox.right; ++j)
			{
				idx = i * width + j;
				addVal = m_terrainData.HeightScale * RAISEDELTA;
				indices.push_back({ i,j,idx });

				//�ִ� ���̸� ������ ���� ��
				if (m_modifyMapOption == 0) //���̸� �ø�
				{
					mHeightmap[idx] = min(m_terrainData.HeightScale,
						mHeightmap[idx] + addVal);
				}
				else //���̸� ����
				{
					mHeightmap[idx] = max(0,
						mHeightmap[idx] - addVal);
				}

				//heightMapData[idx] = XMConvertFloatToHalf(mHeightmap[idx]);
			}
		}
		break;
	case BrushShape::CIRCLE:
		float yCoefficient = 0.0f;
		for (int i = leftTopZ; i < updateBox.bottom; ++i)
		{
			for (int j = leftTopX; j < updateBox.right; ++j)
			{
				idx = i * width + j;

				//�߽ɰ��� �Ÿ�
				float dist = sqrtf(pow((centerTexRow - i), 2) + pow((centerTexCol - j), 2));
				if (dist > radiusTex)
					continue;

				indices.push_back({ i,j,idx });
				//�Ÿ��� ���� 2���Լ� ������� ���̸� ������ų ����� ����(0~1)
				yCoefficient = -(dist*dist) / (radiusTex*radiusTex) + 1;
				addVal = m_terrainData.HeightScale * RAISEDELTA * yCoefficient;
				if (m_modifyMapOption == 0)
				{
					mHeightmap[idx] = min(m_terrainData.HeightScale,
						mHeightmap[idx] + addVal);
				}
				else
				{
					mHeightmap[idx] = max(0,
						mHeightmap[idx] - addVal);
				}
				//�ִ� ���̸� ������ ���� ��	

				//heightMapData[idx] = XMConvertFloatToHalf(mHeightmap[idx]);
			}
		}
		break;
	}


	UINT sizeSum = boxWidth * boxHeight;

	std::vector<HALF> srcTemp(sizeSum, 0);

	UINT srcI, srcJ, srcIdx;
	for (auto& idx : indices)
	{
		srcI = std::get<0>(idx) - updateBox.top;
		srcJ = std::get<1>(idx) - updateBox.left;
		srcIdx = srcI * boxWidth + srcJ;
		Average(std::get<0>(idx), std::get<1>(idx), width);
		srcTemp[srcIdx] = XMConvertFloatToHalf(mHeightmap[std::get<2>(idx)]);
		//heightMapData[std::get<2>(idx)] = XMConvertFloatToHalf(mHeightmap[std::get<2>(idx)]);
	}
	int rowPitch = sizeof(srcTemp[0]) * boxWidth;

	m_texMgr.m_context->UpdateSubresource(m_hmapTex, D3D11CalcSubresource(0, 0, 1),
		&updateBox, &srcTemp[0], rowPitch, 0);
	//m_texMgr.m_context->Unmap(m_hmapTex, D3D11CalcSubresource(0, 0, 1));

}

void TerrainRenderer::ModifyBlendMap(float x, float z)
{
	if (m_brush->shape == BrushShape::NOBRUSH || m_brush->padding[0] < 0)
		return;
	static UINT widthTex = 0;;
	static UINT heightTex = 0;
	//*********** ����� �ؽ��� ����, ������ ������ **********//
	D3D11_MAPPED_SUBRESOURCE mappedData;

	D3D11_TEXTURE2D_DESC blendMapDesc;
	m_blendMapTex->GetDesc(&blendMapDesc);

	static std::vector<UINT> tempBlendMap;
	if (widthTex != blendMapDesc.Width || heightTex != blendMapDesc.Height)
	{
		widthTex = blendMapDesc.Width;
		heightTex = blendMapDesc.Height;
		tempBlendMap.clear();
		tempBlendMap.resize(widthTex * heightTex);
	}




	DXGI_FORMAT format = blendMapDesc.Format;
	//HR(m_texMgr.m_context->Map(m_blendMapTex, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//UCHAR* testArr = static_cast<UCHAR*>(mappedData.pData);


	UCHAR rgba[4] = { 0 };
	rgba[m_selectedMap] = 255;



	// �귯���� �߽ɿ��� ���簢�� ������ �˻�
	float widthW = GetWidth();
	float halfWidthW = widthW * 0.5f;

	float leftTopX, leftTopZ;
	float rightBottomX, rightBottomZ;

	// ������� ��ǥ -> Terrain ���� ��ǥ ��ȯ
	GetLocalPosition(x - m_brush->radius, z + m_brush->radius, &leftTopX, &leftTopZ);
	GetLocalPosition(x + m_brush->radius, z - m_brush->radius, &rightBottomX, &rightBottomZ);

	//terrain ������ǥ -> BlendMap Texture ���� ��ǥ
	UINT startRow = leftTopZ / m_terrainData.HeightmapWidth * widthTex;
	UINT endRow = rightBottomZ / m_terrainData.HeightmapWidth * heightTex;
	UINT startCol = leftTopX / m_terrainData.HeightmapWidth * widthTex;
	UINT endCol = rightBottomX / m_terrainData.HeightmapWidth * heightTex;
	//�ؽ��� ���������� ������
	float radiusTex = (endCol - startCol) * 0.5f;
	float centerColTex = min(startCol + radiusTex, widthTex);
	float centerRowTex = min(startRow + radiusTex, heightTex);
	UINT idx;




	D3D11_BOX updateBox;
	updateBox.top = startRow;
	updateBox.left = startCol;
	updateBox.bottom = endRow;
	updateBox.right = endCol;
	updateBox.front = 0;
	updateBox.back = 1;

	UINT sizeSum = (endCol - startCol) * (endRow - startRow);
	UINT targetBlendValue = 255 << (m_selectedMap * 8);
	std::vector<UINT> srcTemp(sizeSum, 0);

	UINT width = endCol - startCol;
	UINT height = endRow - startRow;
	for (UINT i = 0; i < height; ++i)
	{
		//4���� ���Ҹ� �����ϸ� R8G8B8A8_UNORM�̹Ƿ� ���ȼ� �����ϴ°��� ��.
		//col�� �����ϴ� �迭 �ε����� ���ٿ� width*4
		for (UINT j = 0; j < width; ++j)
		{
			//����ʿ����� �ε��� ���
			idx = (startRow+i)*widthTex + (startCol+j);

			//�귯���� ������ ���� ���� �Ÿ�
			float dist = sqrtf(powf(((startCol +j) - centerColTex), 2) +
				powf(((startRow+i) - centerRowTex), 2));

			//���� �귯���� �� �� ���� ���� ������Ʈ���� ����
			if (m_brush->shape == BrushShape::CIRCLE &&
				dist > radiusTex)
			{
			}
			else
				tempBlendMap[idx] = targetBlendValue;
									
			srcTemp[i * width + j] = tempBlendMap[idx];
		}
	}


	int rowPitch = sizeof(srcTemp[0]) * (endCol - startCol);
	m_texMgr.m_context->UpdateSubresource(m_blendMapTex, D3D11CalcSubresource(0, 0, 1),
		&updateBox, &srcTemp[0], rowPitch, 0);


	//switch (m_brush->shape)
	//{
	//case BrushShape::SQUARE:
	//	//for (UINT i = startRow; i < endRow; ++i)
	//	//{
	//	//	//4���� ���Ҹ� �����ϸ� R8G8B8A8_UNORM�̹Ƿ� ���ȼ� �����ϴ°��� ��.
	//	//	//col�� �����ϴ� �迭 �ε����� ���ٿ� width*4
	//	//	for (UINT j = startCol; j < endCol; ++j)
	//	//	{
	//	//		//UCHAR* �迭�̱� ������ 4�踦 ������� i,j��° �ȼ��� ����
	//	//		idx = (i*width * 4) + j*4;

	//	//		testArr[idx] = r;
	//	//		testArr[idx + 1] = g;
	//	//		testArr[idx + 2] = b;
	//	//		testArr[idx + 3] = a;
	//	//	}
	//	//}
	//	
	//	for (UINT i = 0; i < endRow; ++i)
	//	{
	//		//4���� ���Ҹ� �����ϸ� R8G8B8A8_UNORM�̹Ƿ� ���ȼ� �����ϴ°��� ��.
	//		//col�� �����ϴ� �迭 �ε����� ���ٿ� width*4
	//		for (UINT j = 0; j < endCol; ++j)
	//		{
	//			//UCHAR* �迭�̱� ������ 4�踦 ������� i,j��° �ȼ��� ����
	//			idx = (i*widthTex * 4) + j * 4;

	//			if (startRow <= i && i < endRow && j >= startCol && j < endCol)
	//			{
	//				testArr[idx] = rgba[0];
	//				testArr[idx + 1] = rgba[1];
	//				testArr[idx + 2] = rgba[2];
	//				testArr[idx + 3] = rgba[3];
	//			}
	//			else
	//			{
	//				testArr[idx] = testArr[idx];
	//				testArr[idx + 1] = testArr[idx + 1];
	//				testArr[idx + 2] = testArr[idx + 2];
	//				testArr[idx + 3] = testArr[idx + 3];
	//			}				
	//		}
	//	}



	//	//for (UINT i = 0; (startRow + i) < endRow; ++i)
	//	//{
	//	//	//4���� ���Ҹ� �����ϸ� R8G8B8A8_UNORM�̹Ƿ� ���ȼ� �����ϴ°��� ��.
	//	//	//col�� �����ϴ� �迭 �ε����� ���ٿ� width*4
	//	//	for (UINT j = 0; (startCol+j) < endCol; ++j)
	//	//	{
	//	//		//UCHAR* �迭�̱� ������ 4�踦 ������� i,j��° �ȼ��� ����
	//	//		idx = ((startRow+i)*widthTex * 4) + (startCol+j) * 4;

	//	//		testArr[idx] = rgba[0];
	//	//		testArr[idx + 1] = rgba[1];
	//	//		testArr[idx + 2] = rgba[2];
	//	//		testArr[idx + 3] = rgba[3];
	//	//	}
	//	//}
	//	break;
	//case BrushShape::CIRCLE:
	//	float halfWidth = GetWidth() *0.5f;
	//	float worldX, worldZ;
	//	//for (UINT i = 0; (startRow + i) < endRow; ++i)
	//	//{
	//	//	//4���� ���Ҹ� �����ϸ� R8G8B8A8_UNORM�̹Ƿ� ���ȼ� �����ϴ°��� ��.
	//	//	//col�� �����ϴ� �迭 �ε����� ���ٿ� width*4
	//	//	for (UINT j = 0; (startCol + j) < endCol; ++j)
	//	//	{
	//	//		//UCHAR* �迭�̱� ������ 4�踦 ������� i,j��° �ȼ��� ����
	//	//		idx = ((startRow + i)*widthTex * 4) + (startCol + j) * 4;
	//	//		LocalToWorld((leftTopZ + i), (leftTopX + j), &worldX, &worldZ, halfWidth, halfWidth);

	//	//		//�귯���� ������ ���� ���� �Ÿ�
	//	//		float dist = sqrtf(powf((x - worldX),2) +
	//	//			powf((z - worldZ),2));
	//	//		
	//	//		if (dist > m_brush->radius)
	//	//			continue;

	//	//		testArr[idx] = r;
	//	//		testArr[idx + 1] = g;
	//	//		testArr[idx + 2] = b;
	//	//		testArr[idx + 3] = a;
	//	//	}
	//	//}
	//	for (UINT i = startRow; i < endRow; ++i)
	//	{
	//		//4���� ���Ҹ� �����ϸ� R8G8B8A8_UNORM�̹Ƿ� ���ȼ� �����ϴ°��� ��.
	//		//col�� �����ϴ� �迭 �ε����� ���ٿ� width*4
	//		for (UINT j = startCol; j < endCol; ++j)
	//		{
	//			//UCHAR* �迭�̱� ������ 4�踦 ������� i,j��° �ȼ��� ����
	//			idx = (i*widthTex * 4) + j * 4;
	//			
	//			//�귯���� ������ ���� ���� �Ÿ�
	//			float dist = sqrtf(powf((j - centerColTex), 2) +
	//				powf((i - centerRowTex), 2));

	//			if (dist > radiusTex)
	//				continue;

	//			testArr[idx] = rgba[0];
	//			testArr[idx + 1] = rgba[1];
	//			testArr[idx + 2] = rgba[2];
	//			testArr[idx + 3] = rgba[3];
	//		}
	//	}
	//	break;
	//}

	//m_texMgr.m_context->Unmap(m_blendMapTex, D3D11CalcSubresource(0, 0, 1));
}

void TerrainRenderer::SetHeightScale(float height)
{
	if (m_terrainData.HeightScale == height)
		return;
	m_terrainData.HeightScale = height;
	if (mHeightmap.empty())
		return;
	for (UINT i = 0; i < m_terrainData.HeightmapHeight * m_terrainData.HeightmapWidth; ++i)
	{
		mHeightmap[i] = (mHeightmapDatas[i] / 255.0f)*m_terrainData.HeightScale;
	}
	Init();
}


void TerrainRenderer::SetHeightMap(std::wstring & filePath)
{
	m_terrainData.HeightMapFilename = filePath;
	Init();
	//LoadHeightmap();
	//BuildHeightmapSRV(m_texMgr.md3dDevice);
}

void TerrainRenderer::SetLayerMap(std::wstring & filePath, UINT i)
{
	m_terrainData.layerFilenames[i] = filePath;
	ReleaseCOM(mtextureArraySRV);
	BuildLayermapSRV();

}

void TerrainRenderer::SetBlendMap(std::wstring & filePath)
{
	m_terrainData.BlendMapFilename = filePath;
	BuildBlendmapSRV();
}

void TerrainRenderer::LoadHeightmap()
{
	// A height for each vertex
	//std::vector<unsigned char> in(m_terrainData.HeightmapWidth * m_terrainData.HeightmapHeight);
	mHeightmapDatas.resize(m_terrainData.HeightmapWidth * m_terrainData.HeightmapHeight, 0);

	// Open the file.
	std::ifstream inFile;
	inFile.open(m_terrainData.HeightMapFilename.c_str(), std::ios_base::binary);

	// Copy the array data into a float array and scale it.
	mHeightmap.resize(m_terrainData.HeightmapHeight * m_terrainData.HeightmapWidth, 0);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&mHeightmapDatas[0], (std::streamsize)mHeightmapDatas.size());

		// Done with file.
		inFile.close();

		for (UINT i = 0; i < m_terrainData.HeightmapHeight * m_terrainData.HeightmapWidth; ++i)
		{
			mHeightmap[i] = (mHeightmapDatas[i] / 255.0f)*m_terrainData.HeightScale;
		}
	}

}

//��Ȱȭ
void TerrainRenderer::Smooth()
{
	std::vector<float> dest(mHeightmap.size());

	for (UINT i = 0; i < m_terrainData.HeightmapHeight; ++i)
	{
		for (UINT j = 0; j < m_terrainData.HeightmapWidth; ++j)
		{
			dest[i*m_terrainData.HeightmapWidth + j] = Average(i, j, m_terrainData.HeightmapWidth);
		}
	}

	// Replace the old heightmap with the filtered one.
	mHeightmap = dest;
}

bool TerrainRenderer::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < (int)m_terrainData.HeightmapHeight &&
		j >= 0 && j < (int)m_terrainData.HeightmapWidth;
}

float TerrainRenderer::Average(int i, int j, int width)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
	// and no iterations of the outer for loop occur.
	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += mHeightmap[m*width + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void TerrainRenderer::CalcAllPatchBoundsY()
{
	mPatchBoundsY.resize(mNumPatchQuadFaces);

	// For each patch
	for (UINT i = 0; i < mNumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < mNumPatchVertCols - 1; ++j)
		{
			CalcPatchBoundsY(i, j);
		}
	}
}

void TerrainRenderer::CalcPatchBoundsY(UINT i, UINT j)
{
	// Scan the heightmap values this patch covers and compute the min/max height.

	UINT x0 = j * CellsPerPatch;
	UINT x1 = (j + 1)*CellsPerPatch;

	UINT y0 = i * CellsPerPatch;
	UINT y1 = (i + 1)*CellsPerPatch;

	float minY = +MathHelper::Infinity;
	float maxY = -MathHelper::Infinity;
	for (UINT y = y0; y <= y1; ++y)
	{
		for (UINT x = x0; x <= x1; ++x)
		{
			UINT k = y * m_terrainData.HeightmapWidth + x;
			minY = MathHelper::Min(minY, mHeightmap[k]);
			maxY = MathHelper::Max(maxY, mHeightmap[k]);
		}
	}

	UINT patchID = i * (mNumPatchVertCols - 1) + j;
	mPatchBoundsY[patchID] = XMFLOAT2(minY, maxY);



}

void TerrainRenderer::BuildQuadPatchVB(ID3D11Device* device)
{
	m_patchVertices.resize(mNumPatchVertRows*mNumPatchVertCols);
	m_aabb.resize((mNumPatchVertRows - 1) * (mNumPatchVertCols - 1));

	float halfWidth = 0.5f*GetWidth();
	float halfDepth = 0.5f*GetDepth();

	float patchWidth = GetWidth() / (mNumPatchVertCols - 1);
	float patchDepth = GetDepth() / (mNumPatchVertRows - 1);
	float du = 1.0f / (mNumPatchVertCols - 1);
	float dv = 1.0f / (mNumPatchVertRows - 1);

	for (UINT i = 0; i < mNumPatchVertRows; ++i)
	{
		float z = halfDepth - i * patchDepth;
		for (UINT j = 0; j < mNumPatchVertCols; ++j)
		{
			float x = -halfWidth + j * patchWidth;

			m_patchVertices[i*mNumPatchVertCols + j].Pos = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid.
			m_patchVertices[i*mNumPatchVertCols + j].Tex.x = j * du;
			m_patchVertices[i*mNumPatchVertCols + j].Tex.y = i * dv;
		}
	}


	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for (UINT i = 0; i < mNumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < mNumPatchVertCols - 1; ++j)
		{
			UINT patchID = i * (mNumPatchVertCols - 1) + j;
			UINT vertexID = i * mNumPatchVertCols + j;
			m_patchVertices[vertexID].BoundsY = mPatchBoundsY[patchID];

			//�簢���� �������� ��, ���� �Ʒ� ���� �̿��� aabb�� ���
			XMVECTOR rightTop = XMVectorSet(m_patchVertices[vertexID + 1].Pos.x,
				mPatchBoundsY[patchID].y, //max ����
				m_patchVertices[vertexID + 1].Pos.z, 1.0f);

			UINT leftBottomID = (i + 1)*mNumPatchVertCols + j;
			XMVECTOR leftBottom = XMVectorSet(m_patchVertices[leftBottomID].Pos.x,
				mPatchBoundsY[patchID].x, //min ����
				m_patchVertices[leftBottomID].Pos.z, 1.0f);

			XMStoreFloat3(&m_aabb[patchID].Center, (leftBottom + rightTop)*0.5f);
			XMStoreFloat3(&m_aabb[patchID].Extents, (rightTop - leftBottom)*0.5f);
		}
	}

	mesh->InitVB(device, m_patchVertices);
}

void TerrainRenderer::BuildQuadPatchIB(ID3D11Device* device)
{
	std::vector<UINT> indices(mNumPatchQuadFaces * 4); // 4 indices per quad face

	// Iterate over each quad and compute indices.
	int k = 0;
	for (UINT i = 0; i < mNumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < mNumPatchVertCols - 1; ++j)
		{
			// Top row of 2x2 quad patch
			indices[k] = i * mNumPatchVertCols + j;
			indices[k + 1] = i * mNumPatchVertCols + j + 1;

			// Bottom row of 2x2 quad patch
			indices[k + 2] = (i + 1)*mNumPatchVertCols + j;
			indices[k + 3] = (i + 1)*mNumPatchVertCols + j + 1;

			k += 4; // next quad
		}
	}
	mesh->indices.swap(indices);
	mesh->InitIB(device);
}

void TerrainRenderer::BuildHeightmapSRV(ID3D11Device* device)
{
	ReleaseCOM(mHeightMapSRV);
	ReleaseCOM(m_hmapTex);

	D3D11_TEXTURE2D_DESC texDesc;
	//texDesc.Width = m_terrainData.HeightmapWidth; //basic value 2049
	//texDesc.Height = m_terrainData.HeightmapHeight; //basic value 2049
	texDesc.Width = 2048;
	texDesc.Height = 2048;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	//texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// HALF is defined in xnamath.h, for storing 16-bit float.
	std::vector<HALF> hmap(mHeightmap.size());
	//current mHeightmap is all zero.
	std::transform(mHeightmap.begin(), mHeightmap.end(), hmap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = m_terrainData.HeightmapWidth * sizeof(HALF);
	data.SysMemSlicePitch = 0;

	HR(device->CreateTexture2D(&texDesc, &data, &m_hmapTex));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	HR(device->CreateShaderResourceView(m_hmapTex, &srvDesc, &mHeightMapSRV));

}

void TerrainRenderer::BuildLayermapSRV()
{
	ReleaseCOM(mtextureArraySRV)
		mtextureArraySRV = d3dHelper::CreateTexture2DArraySRV(m_texMgr.md3dDevice, m_texMgr.m_context,
			m_terrainData.layerFilenames,
			DXGI_FORMAT_R8G8B8A8_UNORM);
}

void TerrainRenderer::BuildBlendmapSRV()
{

	if (m_terrainData.BlendMapFilename.empty())
		return;

	//HR(D3DX11CreateShaderResourceViewFromFile(m_texMgr.md3dDevice,
	//	m_terrainData.BlendMapFilename.c_str(), 0, 0, &mblendTextureSRV, 0));

	ReleaseCOM(mblendTextureSRV);
	ReleaseCOM(m_blendMapTex);

	//texture ����
	D3DX11_IMAGE_LOAD_INFO texInfo;
	texInfo.MipLevels = 1;
	texInfo.CpuAccessFlags = 0;
	texInfo.Usage = D3D11_USAGE_DEFAULT;
	texInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texInfo.MiscFlags = 0;


	D3DX11CreateTextureFromFile(m_texMgr.md3dDevice, m_terrainData.BlendMapFilename.c_str(), &texInfo, 0,
		(ID3D11Resource**)&m_blendMapTex, 0);

	HR(m_texMgr.md3dDevice->CreateShaderResourceView(m_blendMapTex, 0, &mblendTextureSRV));



}

