#include "Light.h"

int Lighting::lightCount[4] = { 0 };

void Lighting::SetDiffuse(const XMFLOAT4& diffuse)
{
	m_dirLight.Diffuse = diffuse;
	m_pointLight.Diffuse = diffuse;
	m_spotLight.Diffuse = diffuse;
}

void Lighting::SetAmbient(const XMFLOAT4& ambient)
{
	m_dirLight.Ambient = ambient;
	m_pointLight.Ambient = ambient;
	m_spotLight.Ambient = ambient;
}

void Lighting::SetSpecular(const XMFLOAT4& specular)
{
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
	lightCount[m_lightType]--;
	lightCount[lightType]++;
	m_lightType = lightType;
}

void Lighting::SetDirection(const XMFLOAT3 & direction)
{
	m_dirLight.Direction = direction;
	m_spotLight.Direction = direction;
}

void Lighting::SetRange(float range)
{
	m_pointLight.Range = range;
	m_spotLight.Range = range;
}

void Lighting::SetAtt(const XMFLOAT3& att)
{
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
