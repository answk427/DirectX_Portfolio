#include "Mouse.h"

int Mouse::x = 0;
int Mouse::y = 0;
D3D11_VIEWPORT* Mouse::m_currentViewPort = 0;

void Mouse::GetScreenPos(float * getX, float * getY)
{
	*getX = x;
	*getY = y;
}
