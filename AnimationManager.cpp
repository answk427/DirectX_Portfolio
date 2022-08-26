#include "AnimationManager.h"

AnimationManager& AnimationManager::Instance()
{
	static AnimationManager animationManager;
	return animationManager;
}

void AnimationManager::LoadAnimation(std::map<std::string, AssimpAnimation>& assimpAnimations)
{
	for (auto& clip : assimpAnimations)
	{
		//�̹� �����̸��� Ŭ���� ������ �ǳʶ�
		if (mAnimations.find(clip.first) != mAnimations.end())
			continue;

		MyAnimationClip aniClip;
		ConvertAnimation(clip.first, clip.second, aniClip);
		mAnimations[clip.first] = aniClip;
		
	}
}

void AnimationManager::ConvertAnimation(const std::string & clipName, const AssimpAnimation & assimpAni, MyAnimationClip & dest)
{
	dest.m_clipName = clipName;
	dest.duration = assimpAni.duration;

	for (auto& frame : assimpAni.HierarchyAniClip)
	{
		dest.m_bones.push_back(BoneFrames(frame.m_boneName));
		BoneFrames& boneFrames = dest.m_bones.back();

		std::vector<frameKey3> scaleKeys;
		std::vector<frameKey4> quaternionKeys;
		std::vector<frameKey3> translateKeys;

		for (auto& position : frame.positionKey)
		{
			translateKeys.push_back({ position.time,position.float3Key });
		}
		for (auto& quaternion : frame.quaternionKey)
		{
			quaternionKeys.push_back({ quaternion.time, quaternion.quaternionKey });
		}
		for (auto& scale : frame.scalingKey)
		{
			scaleKeys.push_back({ scale.time, scale.float3Key });
		}
		boneFrames.InitScales(scaleKeys);
		boneFrames.InitQuaternions(quaternionKeys);
		boneFrames.InitTranslations(translateKeys);
	}
}

MyAnimationClip* AnimationManager::GetAnimation(const std::string clipName)
{
	auto it = mAnimations.find(clipName);
	//ã�� �� ���� ��� null��ȯ
	if (it == mAnimations.end())
		return nullptr;

	return &it->second;
}
