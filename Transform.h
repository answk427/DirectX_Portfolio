#pragma once

#include <D3DX11.h>
#include <xnamath.h>

//수정됐는지 안됐는지 확인하기 위한 용도
enum ModifyFlag
{
	MODIFY_POSITION = 1,
	MODIFY_ROTATION = 2,
	MODIFY_SCALE = 4
};

class Transform
{
private:
	int m_modifyFlag;

public:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;
	XMFLOAT4 m_quaternion;
	
	XMFLOAT4X4 m_world;
	XMFLOAT4X4 m_translateMatrix;
	XMFLOAT4X4 m_rotationMatrix;
	XMFLOAT4X4 m_scaleMatrix;
public:
	void* m_owner_obj;

public:
	Transform(void* objectAddr);
	~Transform() {}
public:
	void UpdateWorld();

	void SetPosition(const float& x, const float& y, const float& z);
	void SetPosition(const XMFLOAT3& pos);

	void SetRotation(const float& x, const float& y, const float& z);
	void SetRotation(const XMFLOAT3& rotation);

	void SetScale(const float& x, const float& y, const float& z);
	void SetScale(const XMFLOAT3& scale);

	const XMFLOAT4X4* GetWorld() const { return &m_world; }
	

};	