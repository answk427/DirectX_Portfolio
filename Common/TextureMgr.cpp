#include "TextureMgr.h"

bool TextureMgr::instantiated = false;

TextureMgr::TextureMgr() : md3dDevice(0)
{
	assert(!instantiated);
	instantiated = true;
}

TextureMgr::~TextureMgr()
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
    {
		ReleaseCOM(it->second);
    }

	mTextureSRV.clear();
	instantiated = false;
}

void TextureMgr::Init(ID3D11Device* device)
{
	md3dDevice = device;
}

ID3D11ShaderResourceView* TextureMgr::CreateTexture(std::wstring filename)
{
	USES_CONVERSION;

	if (filename.empty())
		return nullptr;
	//파일이 존재하는지 검사
	else if (_access(W2A(filename.c_str()), 0) < 0)
		return nullptr;

	ID3D11ShaderResourceView* srv = 0;

	// Does it already exist?
	if( mTextureSRV.find(filename) != mTextureSRV.end() )
	{
		srv = mTextureSRV[filename];
	}
	else
	{
		HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, filename.c_str(), 0, 0, &srv, 0 ));
		
		mTextureSRV[filename] = srv;
	}

	return srv;
}
 
