#pragma once

#include <map>
#include "AnimationClass.h"
#include "AssimpLoader.h"

class AnimationManager
{
public:
	static AnimationManager& Instance();
public:
	std::map<std::string, MyAnimationClip> mAnimations;


public:
	void LoadAnimation(std::map<std::string, AssimpAnimation>& assimpAnimations);
	void ConvertAnimation(const std::string& clipName, const AssimpAnimation & assimpAni, MyAnimationClip & dest);
	MyAnimationClip* GetAnimation(const std::string clipName);
};
