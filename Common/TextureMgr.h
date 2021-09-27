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
	TextureMgr();
	~TextureMgr();

	void Init(ID3D11Device* device);

	ID3D11ShaderResourceView* CreateTexture(std::wstring filename);

private:
	TextureMgr(const TextureMgr& rhs);
	TextureMgr& operator=(const TextureMgr& rhs);
	
private:
	ID3D11Device* md3dDevice;
	//std::map<std::wstring, ID3D11ShaderResourceView*> mTextureSRV;
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> mTextureSRV;

	//한개의 인스턴스만 생성되도록 보장하는 변수
	static bool instantiated;
};

#endif // TEXTUREMGR_H

