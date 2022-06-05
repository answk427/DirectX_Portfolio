#pragma once
#include <D3D11.h>

class Mouse
{
public:
	static D3D11_VIEWPORT* m_currentViewPort;
	static int x, y; //½ºÅ©¸° ÁÂÇ¥
	
	static void GetScreenPos(float* getX, float* getY);
	static void UpdateScreenPos(int sx, int sy) { x = sx; y = sy; }
	static void SetViewPort(D3D11_VIEWPORT* viewPort) { m_currentViewPort = viewPort; }
	static D3D11_VIEWPORT* GetViewPort() { return m_currentViewPort; }
};
