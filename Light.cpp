#include "Light.h"

int Lighting::lightCount[4] = { 0 };

bool compareRGBA(const XMFLOAT4 & a, const XMFLOAT4 & b)
{
	if (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w)
		return true;
	return false;
}


void Lighting::SetDiffuse(const XMFLOAT4& diffuse)
{
	if (compareRGBA(m_dirLight.Diffuse, diffuse))
		return;
	m_dirLight.Diffuse = diffuse;
	m_pointLight.Diffuse = diffuse;
	m_spotLight.Diffuse = diffuse;
}

void Lighting::SetAmbient(const XMFLOAT4& ambient)
{
	if (compareRGBA(m_dirLight.Ambient, ambient))
		return;
	m_dirLight.Ambient = ambient;
	m_pointLight.Ambient = ambient;
	m_spotLight.Ambient = ambient;
}

void Lighting::SetSpecular(const XMFLOAT4& specular)
{
	if (compareRGBA(m_dirLight.Specular, specular))
		return;
	m_dirLight.Specular = specular;
	m_pointLight.Specular = specular;
	m_pointLight.Specular = specular;
}

void Lighting::SetBasic(const XMFLOAT4 & diffuse, const XMFLOAT4 & ambient, const XMFLOAT4 & specular)
{
	SetDiffuse(diffuse);
	SetAmbient(ambient);
	SetSpecular(specular);
}

void Lighting::SetLightType(LightType lightType)
{
	if (m_lightType == lightType)
		return;

	lightCount[m_lightType]--;
	lightCount[lightType]++;
	m_lightType = lightType;
}

void Lighting::SetDirection(const XMFLOAT3 & direction)
{
	if (m_dirLight.Direction.x == direction.x &&
		m_dirLight.Direction.y == direction.y &&
		m_dirLight.Direction.z == direction.z)
		return;

	m_dirLight.Direction = direction;
	m_spotLight.Direction = direction;
}

void Lighting::SetRange(float range)
{
	if (m_pointLight.Range == range)
		return;
	m_pointLight.Range = range;
	m_spotLight.Range = range;
}

void Lighting::SetAtt(const XMFLOAT3& att)
{
	if (m_pointLight.Att.x == att.x &&
		m_pointLight.Att.y == att.y &&
		m_pointLight.Att.z == att.z)
		return;

	m_pointLight.Att = att;
	m_spotLight.Att = att;
}

void Lighting::Init()
{
}

void Lighting::FixedUpdate()
{
}

void Lighting::Update()
{
	//조명 위치 업데이트
	m_spotLight.Position = transform->m_position;
	m_pointLight.Position = transform->m_position;
}

void Lighting::LateUpdate()
{
}
