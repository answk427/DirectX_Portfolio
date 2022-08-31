#include "MoveToolRenderer.h"

MoveToolRenderer::MoveToolRenderer(ID3D11Device* device) : effect(0), mVB(0), mIB(0), 
mInstanceBuffer(0),instancingDatas(3), scaleFactor(1.0f), pDSState(0),
mAABBCenter(0.0f,0.0f,0.0f), m_gameObj(0), m_mouseDistance(0.0f,0.0f), m_selectedAxis(-1)
{
	if (effect != nullptr)
		delete effect;
	effect = new MoveToolEffect(device, L"FX/MoveToolEffect.fxo");
	InitCylinderMesh();
	//InitIndices();
	InitInstancingDatas();

	InitVB(device);
	InitIB(device);
	InitInstanceBuffer(device);
	InitDepthStencil(device);
	
}

void MoveToolRenderer::GetMoveToolAxes(XMVECTOR& localPos, XMMATRIX& world, XMMATRIX& viewProj,
	XMVECTOR& desetNDCpos, XMVECTOR& destXAxisNDC, XMVECTOR& destYAxisNDC, XMVECTOR& destZAxisNDC)
{
	XMVECTOR posW = XMVector3TransformCoord(localPos, world);
	XMVECTOR posH = XMVector3TransformCoord(posW, viewProj);

	//동차절단공간에서의 각 축의 끝 점을 구함
	XMVECTOR xAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
		XMMatrixTranslation(1.0f, 0.0f, 0.0f)),	viewProj);
	XMVECTOR yAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
		XMMatrixTranslation(0.0f, 1.0f, 0.0f)), viewProj);
	XMVECTOR zAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
		XMMatrixTranslation(0.0f, 0.0f, 1.0f)), viewProj);
	
	//ndc 공간으로 변환
	desetNDCpos = posH / XMVectorGetZ(posH);
	XMVECTOR xAxisEndPosNDC = xAxisEndPosH/ XMVectorGetZ(xAxisEndPosH);
	XMVECTOR yAxisEndPosNDC = yAxisEndPosH / XMVectorGetZ(yAxisEndPosH);
	XMVECTOR zAxisEndPosNDC = zAxisEndPosH / XMVectorGetZ(zAxisEndPosH);
	
	//ndc공간에서 xyz 축의 방향
	XMVECTOR AxisDir[3];
	AxisDir[0] = xAxisEndPosNDC - desetNDCpos;
	AxisDir[1] = yAxisEndPosNDC - desetNDCpos;
	AxisDir[2] = zAxisEndPosNDC - desetNDCpos;

	float AxisLen[3];
	AxisLen[0] = XMVectorGetX(XMVector2Length(AxisDir[0]));
	AxisLen[1] = XMVectorGetX(XMVector2Length(AxisDir[1]));
	AxisLen[2] = XMVectorGetX(XMVector2Length(AxisDir[2]));

	UINT maxIdx = AxisLen[0] > AxisLen[1] ? 0 : 1;
	maxIdx = AxisLen[maxIdx] > AxisLen[2] ? maxIdx : 2;

	float ratio = AXISSIZE / AxisLen[maxIdx];

	destXAxisNDC = AxisDir[0] * ratio;
	destXAxisNDC = AxisDir[1] * ratio;
	destXAxisNDC = AxisDir[2] * ratio;
}

//void MoveToolRenderer::GetMoveToolAxes(XMVECTOR & posW, XMMATRIX & viewProj, XMVECTOR & desetNDCpos, XMVECTOR & destXAxisNDC, XMVECTOR & destYAxisNDC, XMVECTOR & destZAxisNDC)
//{
//	XMVECTOR posH = XMVector3TransformCoord(posW, viewProj);
//
//	//동차절단공간에서의 각 축의 끝 점을 구함
//	//posW.m128_f32[0] += 1.0f;
//	XMVECTOR xAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
//		XMMatrixTranslation(1.0f, 0.0f, 0.0f)), viewProj);
//	XMVECTOR yAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
//		XMMatrixTranslation(0.0f, 1.0f, 0.0f)), viewProj);
//	XMVECTOR zAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
//		XMMatrixTranslation(0.0f, 0.0f, 1.0f)), viewProj);
//
//	//ndc 공간으로 변환
//	desetNDCpos = posH / XMVectorGetW(posH);
//	XMVECTOR xAxisEndPosNDC = xAxisEndPosH / XMVectorGetW(xAxisEndPosH);
//	XMVECTOR yAxisEndPosNDC = yAxisEndPosH / XMVectorGetW(yAxisEndPosH);
//	XMVECTOR zAxisEndPosNDC = zAxisEndPosH / XMVectorGetW(zAxisEndPosH);
//
//	//ndc공간에서 xyz 축의 방향
//	XMVECTOR AxisDir[3];
//	AxisDir[0] = xAxisEndPosNDC - desetNDCpos;
//	AxisDir[1] = yAxisEndPosNDC - desetNDCpos;
//	AxisDir[2] = zAxisEndPosNDC - desetNDCpos;
//
//	float AxisLen[3];
//	AxisLen[0] = XMVectorGetX(XMVector2Length(AxisDir[0]));
//	AxisLen[1] = XMVectorGetX(XMVector2Length(AxisDir[1]));
//	AxisLen[2] = XMVectorGetX(XMVector2Length(AxisDir[2]));
//
//	UINT maxIdx = AxisLen[0] > AxisLen[1] ? 0 : 1;
//	maxIdx = AxisLen[maxIdx] > AxisLen[2] ? maxIdx : 2;
//
//	float ratio = AXISSIZE / AxisLen[maxIdx];
//
//	destXAxisNDC = AxisDir[0] * ratio;
//	destXAxisNDC = AxisDir[1] * ratio;
//	destXAxisNDC = AxisDir[2] * ratio;
//}

void MoveToolRenderer::GetMoveToolAxes(XMVECTOR & posW, XMMATRIX & viewProj)
{
	XMVECTOR posH = XMVector3TransformCoord(posW, viewProj);

	//동차절단공간에서의 각 축의 끝 점을 구함
	//posW.m128_f32[0] += 1.0f;
	XMVECTOR xAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
		XMMatrixTranslation(1.0f, 0.0f, 0.0f)), viewProj);
	XMVECTOR yAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
		XMMatrixTranslation(0.0f, 1.0f, 0.0f)), viewProj);
	XMVECTOR zAxisEndPosH = XMVector3TransformCoord(XMVector3TransformCoord(posW,
		XMMatrixTranslation(0.0f, 0.0f, 1.0f)), viewProj);

	//ndc 공간으로 변환
	XMVECTOR desetNDCpos = posH / XMVectorGetW(posH);
	XMVECTOR xAxisEndPosNDC = xAxisEndPosH / XMVectorGetW(xAxisEndPosH);
	XMVECTOR yAxisEndPosNDC = yAxisEndPosH / XMVectorGetW(yAxisEndPosH);
	XMVECTOR zAxisEndPosNDC = zAxisEndPosH / XMVectorGetW(zAxisEndPosH);

	//ndc공간에서 xyz 축의 방향
	XMVECTOR AxisDir[3];
	AxisDir[0] = xAxisEndPosNDC - desetNDCpos;
	AxisDir[1] = yAxisEndPosNDC - desetNDCpos;
	AxisDir[2] = zAxisEndPosNDC - desetNDCpos;

	float AxisLen[3];
	AxisLen[0] = XMVectorGetX(XMVector2Length(AxisDir[0]));
	AxisLen[1] = XMVectorGetX(XMVector2Length(AxisDir[1]));
	AxisLen[2] = XMVectorGetX(XMVector2Length(AxisDir[2]));

	UINT maxIdx = AxisLen[0] > AxisLen[1] ? 0 : 1;
	maxIdx = AxisLen[maxIdx] > AxisLen[2] ? maxIdx : 2;

	scaleFactor = AXISSIZE / AxisLen[maxIdx];
}

void MoveToolRenderer::GetAxisVertices(XMVECTOR & ndcPos, XMVECTOR & axisNDC, std::vector<XMVECTOR>& destVertices)
{
	//축에 수직인 벡터
	XMVECTOR perpendicularVec = XMVector2Normalize(XMVector2Orthogonal(axisNDC)) * AXISLINEWIDTH;
	
	/* 정점 5개로 화살표 표현, 1개는 중심점
	  3
	  o
	 / \
  1	o---o 2 
	|   ㅣ
	o---o
         0
	*/
	XMVECTOR vertices[4];
	vertices[0] = ndcPos + perpendicularVec;
	vertices[1] = ndcPos + axisNDC;
	vertices[2] = vertices[1] + perpendicularVec;
	vertices[3] = vertices[1] + perpendicularVec * 0.5f + axisNDC * 0.25f;
	
	for(int i=0; i<4; ++i)
		destVertices.push_back(vertices[i]);
}

void MoveToolRenderer::GetMoveToolVertices(XMFLOAT3 & posW, Camera& camera)
{
	XMVECTOR posNDC, xAxisNDC, yAxisNDC, zAxisNDC;
	//ndc공간에서의 xyz축 벡터를 계산
	//GetMoveToolAxes(XMLoadFloat3(&posW), camera.ViewProj(), posNDC,
	//		xAxisNDC, yAxisNDC, zAxisNDC);
	
	//축에 해당하는 화살표모양을 그리는 정점
	std::vector<XMVECTOR> axisVertices;
	axisVertices.push_back(posNDC);
	GetAxisVertices(posNDC, xAxisNDC, axisVertices);
	GetAxisVertices(posNDC, yAxisNDC, axisVertices);
	GetAxisVertices(posNDC, zAxisNDC, axisVertices);
}

void MoveToolRenderer::InitInstancingDatas()
{
	XMMATRIX identity =  XMMatrixIdentity();
	//y축을 z축에대해 -90도 회전하면 x축
	XMMATRIX makeXaxis = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(-90.0f));
	//y축을 x축에대해 90도 회전하면 z축
	XMMATRIX makeZaxis = XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));

	XMStoreFloat4x4(&instancingDatas[0].world, identity);
	
	//makeXaxis._41 += 5;
	XMStoreFloat4x4(&instancingDatas[1].world, makeXaxis);
	
	//makeZaxis._41 += 10;
	XMStoreFloat4x4(&instancingDatas[2].world, makeZaxis);
	
	//0 :y축 , 1 : x축, 2 : z축
	instancingDatas[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	instancingDatas[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	instancingDatas[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	
	
}

void MoveToolRenderer::InstancingUpdate(ID3D11DeviceContext* context)
{
	if (mInstanceBuffer == nullptr || m_gameObj == nullptr)
		return;
	
	static XMMATRIX identity = XMMatrixIdentity();
	//y축을 z축에대해 -90도 회전하면 x축
	static XMMATRIX makeXaxis = 
		XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(-90.0f));
	//y축을 x축에대해 90도 회전하면 z축
	static XMMATRIX makeZaxis = 
		XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));
	
	//해당 오브젝트의 세계행렬과 AABB의 중심에서 이동값만 추출
	//const XMFLOAT3& center = m_Renderer->GetMesh()->GetAABB().Center;
	const XMFLOAT3& center = mAABBCenter;
	XMMATRIX world;
	//m_Renderer->m_bones.lock()->GetFinalTransform(world, m_Renderer->ownerObjectId);
	m_gameObj->nodeHierarchy->GetFinalTransform(world, m_gameObj->GetID());
	float x = world._41 + center.x;
	float y = world._42 + center.y;
	float z = world._43 + center.z;
	XMMATRIX translate = XMMatrixTranslationFromVector({ x,y,z });
	
	D3D11_MAPPED_SUBRESOURCE mappedData;

	context->Map(mInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	
	InstancingWorldColor* dataView = 
		reinterpret_cast<InstancingWorldColor*>(mappedData.pData);
	
	//스크린에서 항상 같은 크기를 유지하기 위한 scaleFactor만큼 비례변환
	XMMATRIX scaleMat = XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);
	
	XMMATRIX yMatrix = scaleMat * identity * translate;
	XMMATRIX xMatrix = scaleMat * makeXaxis * translate;
	XMMATRIX zMatrix = scaleMat * makeZaxis * translate;
	//인스턴싱 데이터 수정
	XMStoreFloat4x4(&dataView[0].world, yMatrix);
	XMStoreFloat4x4(&dataView[1].world, xMatrix);
	XMStoreFloat4x4(&dataView[2].world, zMatrix);
		

	XMStoreFloat4x4(&instancingDatas[0].world, yMatrix);
	XMStoreFloat4x4(&instancingDatas[1].world, xMatrix);
	XMStoreFloat4x4(&instancingDatas[2].world, zMatrix);

	dataView[0].color = { 0.0f,1.0f,0.0f,1.0f };
	dataView[1].color = { 1.0f,0.0f,0.0f,1.0f };
	dataView[2].color = { 0.0f,0.0f,1.0f,1.0f };

	context->Unmap(mInstanceBuffer, 0);
}

void MoveToolRenderer::InitIndices()
{
	indices.resize(27);
	indices = { 1,0,2,3,1,2,4,3,2,7,0,6,7,5,0,8,7,6,11,0,10,11,9,0,12,11,10 };
}

void MoveToolRenderer::InitCylinderMesh()
{
	GeometryGenerator geo;
	GeometryGenerator::MeshData meshData;
	float radius = 0.03f;
	float height = 1.0f;
	geo.CreateCylinder(radius, radius, height, 10, 10, meshData);
	

	indices.swap(meshData.Indices);
	vertices.resize(meshData.Vertices.size());
	for (UINT i = 0; i< meshData.Vertices.size(); ++i)
	{
		meshData.Vertices[i].Position.y += height * 0.5f;
		vertices[i] = meshData.Vertices[i].Position;
	}

	UINT left, right, farV, nearV, upV;
	float bigRadius = radius * 3;
	float bigHeight = height * 1.2f;
	//막대 위 오각형 뿔 
	vertices.push_back({ -bigRadius, 1.0f, 0.0f }); //왼쪽 정점
	left = vertices.size() - 1;
	
	vertices.push_back({ +bigRadius, 1.0f, 0.0f }); //오른쪽 정점
	right = vertices.size() - 1;
	
	vertices.push_back({ 0.0f, 1.0f, +bigRadius }); //앞쪽 정점
	farV = vertices.size() - 1;

	vertices.push_back({ 0.0f, 1.0f, -bigRadius }); //건너편 정점
	nearV = vertices.size() - 1;

	vertices.push_back({ 0.0f, bigHeight, 0.0f }); //위쪽 정점
	upV = vertices.size() - 1;
	
	
	//오각뿔
	indices.push_back(upV);
	indices.push_back(right);
	indices.push_back(nearV);

	indices.push_back(upV);
	indices.push_back(nearV);
	indices.push_back(left);

	indices.push_back(upV);
	indices.push_back(left);
	indices.push_back(farV);

	indices.push_back(upV);
	indices.push_back(farV);
	indices.push_back(right);

	InitAxisAABB(bigRadius * 2, bigHeight);
}

void MoveToolRenderer::InitVB(ID3D11Device* device)
{
	if (vertices.empty())
		return;
	ReleaseCOM(mVB);

	//bufferDesc 작성
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(vertices[0]) * vertices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //구조적버퍼에 저장된 원소 하나의 크기, 구조적버퍼 사용할때 필요
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &vertices[0];

	HR(device->CreateBuffer(&desc, &subRes, &mVB));
}

void MoveToolRenderer::InitIB(ID3D11Device * device)
{
	if (indices.empty())
		return;
	ReleaseCOM(mIB);

	//bufferDesc 작성
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * indices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0; //구조적버퍼에 저장된 원소 하나의 크기, 구조적버퍼 사용할때 필요
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &indices[0];

	HR(device->CreateBuffer(&desc, &subRes, &mIB));
}

void MoveToolRenderer::InitInstanceBuffer(ID3D11Device * device)
{
	ReleaseCOM(mInstanceBuffer);

	//bufferDesc 작성
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(instancingDatas[0]) * 3;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //쓰기 가능 버퍼
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	
	

	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = &instancingDatas[0];
	//매핑후 instancingData의 내용을 직접 쓸것이므로 subresource는 null
	HR(device->CreateBuffer(&desc, &subRes, &mInstanceBuffer));
}

void MoveToolRenderer::InitDepthStencil(ID3D11Device * device)
{
	//깊이판정 비활성화
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	
	device->CreateDepthStencilState(&dsDesc, &pDSState);
}

void MoveToolRenderer::Draw(ID3D11DeviceContext * context, Camera* camera)
{
	if (m_gameObj == nullptr)
		return;

	GetMoveToolAxes(XMVectorZero(), camera->ViewProj());
	InstancingUpdate(context);
		
	
	effect->IASetting(context, 0);
				

	ID3D11Buffer* vbs[2] = { mVB, mInstanceBuffer };
	UINT strides[2] = { sizeof(vertices[0]), sizeof(instancingDatas[0]) };
	UINT offsets[2] = { 0,0 };

	context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	context->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	

	ID3DX11EffectTechnique* tech = effect->GetTechnique(0);
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	
	effect->SetCamera(camera);
	
	context->OMSetDepthStencilState(pDSState, 0);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexedInstanced(indices.size(), 3, 0, 0, 0);
	}
	context->OMSetDepthStencilState(0, 0);
}

bool MoveToolRenderer::SetObject(GameObject * gameObj)
{
	if (gameObj == nullptr)
		return false;

	InitObject();
	m_gameObj = gameObj;

	MeshRenderer* renderer = dynamic_cast<MeshRenderer*>(gameObj->GetComponent(ComponentType::MESHRENDERER));
	SkinnedMeshRenderer* skinnedRenderer = dynamic_cast<SkinnedMeshRenderer*>(gameObj->GetComponent(ComponentType::SKINNEDMESHRENDERER));

	

	GetMaxAABB(gameObj);
}

void MoveToolRenderer::GetMaxAABB(GameObject * gameObj)
{
	MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(gameObj->GetComponent(ComponentType::MESHRENDERER));
	SkinnedMeshRenderer* skinnedRenderer = dynamic_cast<SkinnedMeshRenderer*>(gameObj->GetComponent(ComponentType::SKINNEDMESHRENDERER));

	Renderer* renderer = nullptr;

	if (meshRenderer != nullptr)
		renderer = meshRenderer;
	else if (skinnedRenderer != nullptr)
		renderer = skinnedRenderer;

	if (renderer != nullptr)
	{
		XNA::AxisAlignedBox& aabb = renderer->GetMesh()->GetAABB();
		XMVECTOR extents = XMLoadFloat3(&aabb.Extents);
		float length = XMVectorGetX(XMVector3Length(extents));
		
		mAABBCenter = length > maxLength ? aabb.Center : mAABBCenter;

		maxLength = max(length, maxLength);
	}

	for (auto child : gameObj->childs)
	{
		GetMaxAABB((GameObject*)child);
	}
}

void MoveToolRenderer::InitObject()
{
	mAABBCenter = { 0.0f,0.0f,0.0f };
	maxLength = 0.0f;
}

void MoveToolRenderer::InitAxisAABB(float width, float height)
{
	mAABB.Center = XMFLOAT3(0, height*0.5f, 0);
	mAABB.Extents = XMFLOAT3(width*0.5f, height*0.5f, width*0.5f);
}

void MoveToolRenderer::AxisAction(UINT idx, XMMATRIX & viewProj)
{
	if (m_gameObj == nullptr)
		return;
	
	std::shared_ptr tr = m_gameObj->transform;
	if (!tr)
		return;

	XMMATRIX toHomogeneous = XMLoadFloat4x4(&instancingDatas[idx].world) *
		viewProj;
	//동차 절단공간으로 변환
	
	XMVECTOR axisStart = XMVector3TransformCoord(XMVectorSet(0.0f,0.0f,0.0f,1.0f),
		toHomogeneous);
	XMVECTOR axisEnd = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
		toHomogeneous);
	
	float force = CalcForce(m_mouseDistance, axisStart, axisEnd);
	
	//y축
	if (idx == 0)
		tr->MovePosition(0.0f, force, 0.0f);
	//x축
	else if(idx == 1)
		tr->MovePosition(force, 0.0f, 0.0f);
	//z축
	else
		tr->MovePosition(0.0f, 0.0f, force);

	m_mouseDistance = XMFLOAT2(0.0f, 0.0f);
	
}

void MoveToolRenderer::AxisAction(XMMATRIX & viewProj)
{
	if (m_selectedAxis != -1)
		AxisAction(m_selectedAxis, viewProj);
}

float MoveToolRenderer::CalcForce(XMFLOAT2 & mouseDiff, const XMVECTOR& axisStartH,
	const XMVECTOR& axisEndH)
{
	//동차절단공간 -> ndc 좌표
	XMVECTOR startNDC = axisStartH / XMVectorGetW(axisStartH);
	startNDC.m128_f32[2] = 0.0f;
	startNDC.m128_f32[3] = 0.0f;
	//XMFLOAT2 startNDC(axisStartH.x / axisStartH.w, axisStartH.y / axisStartH.w);
	//XMFLOAT2 endNDC(axisEndH.x / axisEndH.w, axisEndH.y / axisEndH.w);
	XMVECTOR endNDC = axisEndH / XMVectorGetW(axisEndH);
	endNDC.m128_f32[2] = 0.0f;
	endNDC.m128_f32[3] = 0.0f;
	//XMFLOAT2 axisNDC(endNDC.x - startNDC.x, endNDC.y - startNDC.y);
	XMVECTOR axisNDC = endNDC - startNDC;
	
	//XMVECTOR axisNDCvec = XMLoadFloat2(&axisNDC);
	
	//단위길이로 정규화
	axisNDC = XMVector2Normalize(axisNDC);
	
	
	XMVECTOR mouseDiffvec = XMLoadFloat2(&mouseDiff);
	//마우스 이동 벡터에서 단위벡터 축 방향의 벡터를 추출
	return XMVectorGetX(XMVector2Dot(mouseDiffvec, axisNDC)) * FORCEFACTOR;
}

void MoveToolRenderer::SetMouseDistance(float dx, float dy)
{
	m_mouseDistance.x = dx;
	m_mouseDistance.y = dy;
}

bool MoveToolRenderer::AxisIntersect(D3D11_VIEWPORT * viewPort, Camera* camera, float sx, float sy)
{
	XMVECTOR rayOrigin;
	XMVECTOR rayDir;
	XMMATRIX world;
	//스크린좌표 -> 시야공간 반직선 계산
	RayPicking::ScreenToViewRay(&rayOrigin, &rayDir, sx, sy, viewPort, &camera->Proj());

	for (int i = 0; i < 3; ++i)
	{
		std::pair<XMVECTOR, XMVECTOR> localRay;
		world = XMLoadFloat4x4(&instancingDatas[i].world);
		localRay = RayPicking::ViewToLocalRay(&rayOrigin, &rayDir, &camera->View(),
			&world);

		if (XMVector3IsNaN(localRay.first) || XMVector3IsNaN(localRay.second))
			return false;

		//반직선과 AABB 교차판정
		float t = 0.0f;
		if (XNA::IntersectRayAxisAlignedBox(localRay.first, localRay.second, &mAABB, &t))
		{
			//교차하는 축이 있으면 이동
			AxisAction(i, camera->ViewProj());
			//현재 선택된 축 설정
			m_selectedAxis = i;
			return true;
		}
	}
	return false;
}







