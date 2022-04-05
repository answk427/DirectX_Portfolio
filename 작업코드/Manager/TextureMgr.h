#ifndef TEXTUREMGR_H
#define TEXTUREMGR_H

#include "d3dUtil.h"
#include <map>
#include <unordered_map>
#include <io.h>
#include <atlconv.h>


///<summary>
/// Simple texture manager to avoid loading duplicate textures from file.  That can
/// happen, for example, if multiple meshes reference the same texture filename. 
///</summary>
class TextureMgr
{
public:
	static TextureMgr& Instance();
	TextureMgr();
	~TextureMgr();

	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	bool FileCheck(const std::wstring& fileName);

	ID3D11ShaderResourceView* CreateTexture(const std::wstring& filename);

	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* m_context;
private:
	TextureMgr(const TextureMgr& rhs);
	//TextureMgr& operator=(const TextureMgr& rhs);
	
private:
	//std::map<std::wstring, ID3D11ShaderResourceView*> mTextureSRV;
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> mTextureSRV;

	
};

#endif // TEXTUREMGR_H

