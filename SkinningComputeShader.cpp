#include "SkinningComputeShader.h"


void SkinningComputeShader::InitSkinDataBuffer(const std::vector<MyVertex::SkinnedData>& skinDatas, ID3D11Device * device)
{
	ReleaseCOM(skinDataSRV);
	ID3D11Buffer* skinDataBuffer;


	D3D11_BUFFER_DESC skinDataDesc;
	skinDataDesc.Usage = D3D11_USAGE_DEFAULT;
	skinDataDesc.ByteWidth = sizeof(MyVertex::SkinnedData) * skinDatas.size();
	skinDataDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	skinDataDesc.CPUAccessFlags = 0;
	skinDataDesc.StructureByteStride = sizeof(MyVertex::SkinnedData);
	skinDataDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA skinRes;
	skinRes.pSysMem = &skinDatas[0];
	HR(device->CreateBuffer(&skinDataDesc, &skinRes, &skinDataBuffer));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = skinDatas.size();

	HR(device->CreateShaderResourceView(skinDataBuffer, &srvDesc, &skinDataSRV));

	ReleaseCOM(skinDataBuffer);
}



void SkinningComputeShader::InitComputedVertexSRV(ID3D11Device * device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = bufferSize;

	HR(device->CreateShaderResourceView(RWvertexBuffer, &srvDesc, &computedVertexSRV));
}

void SkinningComputeShader::Execute(ID3D11DeviceContext * context, Camera * camera, XMMATRIX & world, std::vector<XMFLOAT4X4>& boneTransforms)
{

	// HORIZONTAL blur pass
	D3DX11_TECHNIQUE_DESC techDesc;
	
	computeSkinningShader->computeSkinningTech->GetDesc(&techDesc);
	
	computeSkinningShader->PerObjectSet(camera, world);
	computeSkinningShader->SetBoneTransforms(&boneTransforms[0], boneTransforms.size());
	computeSkinningShader->SetVertexSrcData(vertexSRV);
	computeSkinningShader->SetVertexDestData(vertexUAV);
	computeSkinningShader->SetSkinData(skinDataSRV);
	
	
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{		
		computeSkinningShader->computeSkinningTech->GetPassByIndex(p)->Apply(0, context);
				
		UINT numGroupsX = (UINT)ceilf(bufferSize / 256.0f);
		context->Dispatch(numGroupsX, 1, 1);
	}

	// Unbind the input texture from the CS for good housekeeping.
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	context->CSSetShaderResources(0, 1, nullSRV);

	// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
	// and a resource cannot be both an output and input at the same time.
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


	// Disable compute shader.
	context->CSSetShader(0, 0, 0);
}
