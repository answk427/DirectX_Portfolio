#include "Transform.h"

Transform::Transform(void* objectAddr) :m_position(0, 0, 0), m_rotation(0, 0, 0), m_scale(1, 1, 1), m_modifyFlag(0), m_owner_obj(objectAddr) 
{
	XMMATRIX tempMat = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMStoreFloat4x4(&m_scaleMatrix, tempMat);

	tempMat = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMStoreFloat4x4(&m_translateMatrix, tempMat);

	m_modifyFlag |= ModifyFlag::MODIFY_ROTATION;
	UpdateWorld();
}

void Transform::UpdateWorld()
{
	if (m_modifyFlag == 0)
		return;

	//Scale
	if (m_modifyFlag && ModifyFlag::MODIFY_SCALE)
	{
		m_scaleMatrix._11 = m_scale.x;
		m_scaleMatrix._22 = m_scale.y;
		m_scaleMatrix._33 = m_scale.z;
	}
	XMMATRIX scaleMat = XMLoadFloat4x4(&m_scaleMatrix);

	//Rotation
	XMMATRIX rotationMat;
	if (m_modifyFlag && ModifyFlag::MODIFY_ROTATION)
	{
		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_rotation.x), XMConvertToRadians(m_rotation.y),XMConvertToRadians(m_rotation.z));
		XMStoreFloat4(&m_quaternion, quat);
		//쿼터니언을 회전행렬로 변환
		rotationMat = XMMatrixRotationQuaternion(quat);
		XMStoreFloat4x4(&m_rotationMatrix, rotationMat);
	}
	else
		rotationMat = XMLoadFloat4x4(&m_rotationMatrix);

	//Translate
	if (m_modifyFlag && ModifyFlag::MODIFY_POSITION)
	{
		m_translateMatrix._41 = m_position.x;
		m_translateMatrix._42 = m_position.y;
		m_translateMatrix._43 = m_position.z;
	}
	XMMATRIX translateMat = XMLoadFloat4x4(&m_translateMatrix);
	
	//world
	XMMATRIX worldMat = scaleMat * rotationMat * translateMat;
	XMStoreFloat4x4(&m_world, worldMat);

	m_modifyFlag = 0;
}

void Transform::SetPosition(const float & x, const float & y, const float & z)
{
	//갱신하려는 정보가 현재와 같은경우 갱신할 필요가 없음.
	if (m_position.x == x && m_position.y == y && m_position.z == z)
		return;
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	m_modifyFlag |= ModifyFlag::MODIFY_POSITION;

}

void Transform::SetPosition(const XMFLOAT3 & pos)
{
	//갱신하려는 정보가 현재와 같은경우 갱신할 필요가 없음.
	if (m_position.x == pos.x && m_position.y == pos.y && m_position.z == pos.z)
		return;
	
	 m_position = pos;
	 m_modifyFlag |= ModifyFlag::MODIFY_POSITION; 
}

void Transform::SetRotation(const float & x, const float & y, const float & z)
{
	if (m_rotation.x == x && m_rotation.y == y && m_rotation.z == z)
		return;
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
	m_modifyFlag |= ModifyFlag::MODIFY_ROTATION;

}

void Transform::SetRotation(const XMFLOAT3 & rotation)
{
	if (m_rotation.x == rotation.x && m_rotation.y == rotation.y && m_rotation.z == rotation.z)
		return;
	 m_rotation = rotation; 
	 m_modifyFlag |= ModifyFlag::MODIFY_ROTATION;
}


void Transform::SetScale(const float & x, const float & y, const float & z)
{
	if (m_scale.x == x && m_scale.y == y && m_scale.z == z)
		return;
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
	m_modifyFlag |= ModifyFlag::MODIFY_SCALE;

}

void Transform::SetScale(const XMFLOAT3 & scale)
{
	if (m_scale.x == scale.x && m_scale.y == scale.y && m_scale.z == scale.z)
		return;
	 m_scale = scale; 
	 m_modifyFlag |= ModifyFlag::MODIFY_SCALE;
}
