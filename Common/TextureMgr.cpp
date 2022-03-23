#include "TextureMgr.h"


TextureMgr & TextureMgr::Instance()
{
	static TextureMgr* instance = new TextureMgr();
	return *instance;
}

TextureMgr::TextureMgr() : md3dDevice(0), m_context(0)
{
	
}

TextureMgr::~TextureMgr()
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
    {
		ReleaseCOM(it->second);
    }

	mTextureSRV.clear();
	
}

void TextureMgr::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	md3dDevice = device;
	m_context = context;
}

bool TextureMgr::FileCheck(const std::wstring & fileName)
{
	USES_CONVERSION;

	if (fileName.empty())
		return false;
	//파일이 존재하는지 검사
	else if (_access(W2A(fileName.c_str()), 0) < 0)
		return false;

	return true;
}

ID3D11ShaderResourceView* TextureMgr::CreateTexture(const std::wstring& filename)
{
	USES_CONVERSION;

	if (!FileCheck(filename))
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
 
