#pragma once

#include <d3dUtil.h>
#include <Camera.h>
#include <BufferResource.h>
#include "Effects.h"
#include "EffectMgr.h"

class SkinningComputeShader
{
public:
	std::wstring fileDirectory;
	SkinningComputeShader(const std::wstring& filename, ID3D11Device* device) :
		fileDirectory(EffectMgr::Instance().fileDirectory),
		computeSkinningShader(new ComputeSkinningEffect(device, fileDirectory + filename)),
		skinDataSRV(0), vertexUAV(0), boneTransformsSRV(0), vertexSRV(0), RWvertexBuffer(0)
	{}
	~SkinningComputeShader()
	{
		ReleaseCOM(vertexSRV);
		ReleaseCOM(skinDataSRV);
		ReleaseCOM(vertexUAV);
		ReleaseCOM(boneTransformsSRV);
		ReleaseCOM(RWvertexBuffer);
	}



public:
	ID3D11ShaderResourceView* computedVertexSRV;
	ID3D11Buffer* RWvertexBuffer;
	int bufferSize = 0;
private:
	ComputeSkinningEffect* computeSkinningShader;
private:
	//계산 결과값을 기록할 버퍼 uav
	ID3D11UnorderedAccessView* vertexUAV;
	//쉐이더 자원으로 묶을 정점데이터, 최종변환행렬
	ID3D11ShaderResourceView* vertexSRV;
	ID3D11ShaderResourceView* skinDataSRV;
	ID3D11ShaderResourceView* boneTransformsSRV;

	int vertexSize = 0;
public:
	//template <typename vertexType>
	//void InitRWvertexBuffer(const std::vector<vertexType>& vertices, ID3D11Device* device);

	//메쉬의 정점들로 버퍼,SRV 초기화
	template <typename vertexType>
	void InitVertexSRV(const std::vector<vertexType>& vertices, ID3D11Device* device);
	
	template <typename vertexType>
	void InitVertexUAV(ID3D11Device* device);

	void InitSkinDataBuffer(const std::vector<MyVertex::SkinnedData>& skinDatas, ID3D11Device* device);

	//렌더링 할때 정점버퍼 대신 바인딩 할 SRV 생성
	void InitComputedVertexSRV(ID3D11Device* device);
	void Execute(ID3D11DeviceContext* context, Camera* camera, XMMATRIX& world, std::vector<XMFLOAT4X4>& boneTransforms);
};

//template<typename vertexType>
//inline void SkinningComputeShader::InitRWvertexBuffer(const std::vector<vertexType>& vertices, ID3D11Device* device)
//{
//	D3D11_BUFFER_DESC rwVertexDesc;
//	rwVertexDesc.Usage = D3D11_USAGE_DEFAULT;
//	rwVertexDesc.ByteWidth = sizeof(vertexType) * vertices.size();
//	//structure buffer는 vertex buffer bindflag 사용불가.
//	//다른 조합 시도해보기. miscflag,bindflag
//	//rwVertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
//	rwVertexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
//	rwVertexDesc.CPUAccessFlags = 0;
//	rwVertexDesc.StructureByteStride = sizeof(vertexType);
//	rwVertexDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; 
//	
//	
//
//	D3D11_SUBRESOURCE_DATA vertexRes;
//	vertexRes.pSysMem = &vertices[0];
//	
//	HR(device->CreateBuffer(&rwVertexDesc, &vertexRes, &RWvertexBuffer));
//}

template<typename vertexType>
inline void SkinningComputeShader::InitVertexSRV(const std::vector<vertexType>& vertices, ID3D11Device * device)
{
	ReleaseCOM(vertexSRV);

	ID3D11Buffer* vertexDataBuffer;


	D3D11_BUFFER_DESC vertexDataDesc;
	vertexDataDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDataDesc.ByteWidth = sizeof(vertexType) * vertices.size();
	vertexDataDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	vertexDataDesc.CPUAccessFlags = 0;
	vertexDataDesc.StructureByteStride = sizeof(vertexType);
	vertexDataDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA vertexRes;
	vertexRes.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vertexDataDesc, &vertexRes, &vertexDataBuffer));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = vertices.size();

	HR(device->CreateShaderResourceView(vertexDataBuffer, &srvDesc, &vertexSRV));

	ReleaseCOM(vertexDataBuffer);

	bufferSize = vertices.size();
	vertexSize = sizeof(vertexType);
}

template<typename vertexType>
inline void SkinningComputeShader::InitVertexUAV(ID3D11Device * device)
{
	ReleaseCOM(vertexUAV);

	D3D11_BUFFER_DESC rwVertexDesc;
	rwVertexDesc.Usage = D3D11_USAGE_DEFAULT;
	
	rwVertexDesc.ByteWidth = sizeof(vertexType)* bufferSize;
	rwVertexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	rwVertexDesc.CPUAccessFlags = 0;
	
	rwVertexDesc.StructureByteStride = sizeof(vertexType);
	rwVertexDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	HR(device->CreateBuffer(&rwVertexDesc, 0, &RWvertexBuffer));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = bufferSize;

	HR(device->CreateUnorderedAccessView(RWvertexBuffer, &uavDesc,
		&vertexUAV));

}



