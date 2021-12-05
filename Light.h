#pragma once

#include "Interface.h"


class Light : public Component
{
public:
	Light(std::string name, ComponentType lightType) : Component(name, lightType) {}
};
