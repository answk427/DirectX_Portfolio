//***************************************************************************************
// d3dUtil.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "d3dUtil.h"
#include "TextureMgr.h"

ID3D11ShaderResourceView* d3dHelper::CreateTexture2DArraySRV(
	ID3D11Device* device, ID3D11DeviceContext* context,
	std::vector<std::wstring>& filenames,
	DXGI_FORMAT format,
	UINT filter,
	UINT mipFilter)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//
	TextureMgr& texMgr = TextureMgr::Instance();

	UINT size = filenames.size();
	
	int enableTexture = -1;

	std::vector<ID3D11Texture2D*> srcTex(size);
	
	for (int i = size-1; i >= 0; --i)
	{
		//file이 유효하지 않다면 건너뜀
		if (!texMgr.FileCheck(filenames[i]))
		{
			srcTex[i] = nullptr;
			continue;
		}
		//유효하다면 사용가능 텍스쳐의 인덱스로 설정
		enableTexture = i;

		D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = format;
		loadInfo.Filter = filter;
		loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo = 0;

		HR(D3DX11CreateTextureFromFile(device, filenames[i].c_str(),
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));
	}
	
	//유효한 텍스쳐가 1개도 없으므로 return
	if (enableTexture == -1)
		return nullptr;
	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[enableTexture]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	HR(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		if (srcTex[texElement] == nullptr)
			continue;
		// for each mipmap level...
		D3D11_TEXTURE2D_DESC currElementDesc;
		srcTex[texElement]->GetDesc(&currElementDesc);
		
		//더 작은 수준의 miplevel 까지만 적재
		UINT minMipLevels = min(currElementDesc.MipLevels, texArrayDesc.MipLevels);
		
		for (UINT mipLevel = 0; mipLevel < minMipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(srcTex[texElement], mipLevel);
		}
	}

	//
	// Create a resource view to the texture array.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);

	for (UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return texArraySRV;
}

bool d3dHelper::ModifyTexture2DArraySRV(ID3D11Device * device, ID3D11DeviceContext * context,
	ID3D11ShaderResourceView * destTextureArray, UINT destIdx,
	const std::wstring & newFileName, DXGI_FORMAT format, UINT filter, UINT mipFilter)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//
	if (destTextureArray == nullptr)
		return false;

	//텍스쳐배열에 덮어쓰기 할 새로운 텍스쳐 load
	ID3D11Texture2D* srcTex;
	D3DX11_IMAGE_LOAD_INFO loadInfo;

	loadInfo.Width = D3DX11_FROM_FILE;
	loadInfo.Height = D3DX11_FROM_FILE;
	loadInfo.Depth = D3DX11_FROM_FILE;
	loadInfo.FirstMipLevel = 0;
	loadInfo.MipLevels = D3DX11_FROM_FILE;
	loadInfo.Usage = D3D11_USAGE_STAGING;
	loadInfo.BindFlags = 0;
	loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	loadInfo.MiscFlags = 0;
	loadInfo.Format = format;
	loadInfo.Filter = filter;
	loadInfo.MipFilter = mipFilter;
	loadInfo.pSrcInfo = 0;

	HR(D3DX11CreateTextureFromFile(device, newFileName.c_str(),
		&loadInfo, 0, (ID3D11Resource**)&srcTex, 0));


	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	//새로 교체할 텍스쳐의 정보
	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex->GetDesc(&texElementDesc);

	//텍스쳐 배열의 정보
	ID3D11Resource* textureArr;
	destTextureArray->GetResource(&textureArr);
	ID3D11Texture2D* texture2d;
	HR(textureArr->QueryInterface(&texture2d));

	D3D11_TEXTURE2D_DESC textureArrDesc;
	texture2d->GetDesc(&textureArrDesc);

	//textureArray의 원소 크기와 새로운 texture의 크기가 같지 않으면 리턴
	if ((texElementDesc.Width != textureArrDesc.Width) || (texElementDesc.Height != textureArrDesc.Height))
		return false;



	//
	// Copy individual texture elements into texture array.
	//

	// for each mipmap level...
	D3D11_TEXTURE2D_DESC currElementDesc;
	srcTex->GetDesc(&currElementDesc);

	//더 작은 수준의 miplevel 까지만 적재
	UINT minMipLevels = min(currElementDesc.MipLevels, textureArrDesc.MipLevels);

	//매개변수로 주어진 idx번째 arraySlice를 변경
	for (UINT mipLevel = 0; mipLevel < minMipLevels; ++mipLevel)
	{
		D3D11_MAPPED_SUBRESOURCE mappedTex2D;
		HR(context->Map(srcTex, mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

		context->UpdateSubresource(textureArr,
			D3D11CalcSubresource(mipLevel, destIdx, textureArrDesc.MipLevels),
			0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

		context->Unmap(srcTex, mipLevel);
	}

		
	ReleaseCOM(textureArr);
	ReleaseCOM(texture2d);
	ReleaseCOM(srcTex);

	return true;
}

ID3D11ShaderResourceView* d3dHelper::CreateRandomTexture1DSRV(ID3D11Device* device)
{
	// 
	// Create the random data.
	//
	XMFLOAT4 randomValues[1024];

	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* randomTexSRV = 0;
	HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));

	ReleaseCOM(randomTex);

	return randomTexSRV;
}

void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M)
{
	//
	// Left
	//
	planes[0].x = M(0, 3) + M(0, 0);
	planes[0].y = M(1, 3) + M(1, 0);
	planes[0].z = M(2, 3) + M(2, 0);
	planes[0].w = M(3, 3) + M(3, 0);

	//
	// Right
	//
	planes[1].x = M(0, 3) - M(0, 0);
	planes[1].y = M(1, 3) - M(1, 0);
	planes[1].z = M(2, 3) - M(2, 0);
	planes[1].w = M(3, 3) - M(3, 0);

	//
	// Bottom
	//
	planes[2].x = M(0, 3) + M(0, 1);
	planes[2].y = M(1, 3) + M(1, 1);
	planes[2].z = M(2, 3) + M(2, 1);
	planes[2].w = M(3, 3) + M(3, 1);

	//
	// Top
	//
	planes[3].x = M(0, 3) - M(0, 1);
	planes[3].y = M(1, 3) - M(1, 1);
	planes[3].z = M(2, 3) - M(2, 1);
	planes[3].w = M(3, 3) - M(3, 1);

	//
	// Near
	//
	planes[4].x = M(0, 2);
	planes[4].y = M(1, 2);
	planes[4].z = M(2, 2);
	planes[4].w = M(3, 2);

	//
	// Far
	//
	planes[5].x = M(0, 3) - M(0, 2);
	planes[5].y = M(1, 3) - M(1, 2);
	planes[5].z = M(2, 3) - M(2, 2);
	planes[5].w = M(3, 3) - M(3, 2);

	// Normalize the plane equations.
	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}

bool compareRGBA(const XMFLOAT4 & a, const XMFLOAT4 & b)
{
	if (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w)
		return true;
	return false;
}

bool compareRGB(const XMFLOAT3 & a, const XMFLOAT3 & b)
{
	if (a.x == b.x && a.y == b.y && a.z == b.z)
		return true;
	return false;
}
