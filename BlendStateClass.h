#pragma once
#include <d3dUtil.h>

class BlendStateInterface
{
public:
	virtual void Init() = 0;
public:
	ID3D11BlendState* m_blendState;
	BlendStateInterface() :m_blendState(0) { Init();  }
};

class BasicBlendState : BlendStateInterface
{
	// BlendStateInterface��(��) ���� ��ӵ�
	virtual void Init() override;
};
