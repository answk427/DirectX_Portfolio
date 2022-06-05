#pragma once

#include "Interface.h"
#include <LightHelper.h>
#include "Transform.h"



enum LightType
{
	DIRECTIONAL,
	POINTLIGHT,
	SPOTLIGHT
};


class Lighting : public Component
{
private:
	LightType m_lightType;
	DirectionalLight m_dirLight;
	PointLight m_pointLight;
	SpotLight m_spotLight;
	

public:
	static int lightCount[4];
	Transform* transform;

public:

	Lighting(std::string name) : Component(name, ComponentType::LIGHT), m_lightType(LightType::DIRECTIONAL)
	{}
	
	Lighting(const Lighting& other) : Component(other.id, ComponentType::LIGHT)
	{
		m_lightType = other.m_lightType;
		m_dirLight = other.m_dirLight;
		m_pointLight = other.m_pointLight;
		m_spotLight = other.m_spotLight;
	}
	~Lighting() {}
	

	void SetDiffuse(const XMFLOAT4& diffuse);
	void SetAmbient(const XMFLOAT4& ambient);
	void SetSpecular(const XMFLOAT4& specular);
	void SetBasic(const XMFLOAT4& diffuse, const XMFLOAT4& ambient, const XMFLOAT4& specular);

	void SetLightType(LightType lightType);

	//빛의 방향 설정
	void SetDirection(const XMFLOAT3& direction);
	//빛의 범위 설정
	void SetRange(float range);
	//거리에 따른 감쇠 매개변수(attenuation)
	void SetAtt(const XMFLOAT3& att);
	//spot Light 에서 spot만큼 제곱이 됨
	void SetSpot(float spot);
	//transform 설정
	void SetTransform(Transform* tr) { transform = tr; }
		
public:
	const DirectionalLight& GetDirLight() const { return m_dirLight; }
	DirectionalLight& GetDirLight() { return m_dirLight; }
	const PointLight& GetPointLight() const { return m_pointLight; }
	const SpotLight& GetSpotLight() const { return m_spotLight; }
	LightType GetLightType() const { return m_lightType; }
		
public:
	// Component을(를) 통해 상속됨
	virtual void Init() override;
	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

	// Component을(를) 통해 상속됨
	virtual void Enable() override;
	virtual void Disable() override;
};
