#include "AnimationClass.h"

void BoneFrames::Interpolate(float time, XMFLOAT4X4& dest)
{
	XMVECTOR scale;
	XMVECTOR quaternion;
	XMVECTOR translate;
	float lerpPercent;
	

	if (scaleKeys.empty())
		scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	else if (scaleKeys.size() == 1 || time <= scaleKeys[0].first)
		scale = XMLoadFloat3(&scaleKeys[0].second);
	else if(time >= scaleKeys.back().first)
		scale = XMLoadFloat3(&scaleKeys.back().second);
	else
	{
		//time���� ũ�ų� ���� Ű�������� ã�� �����Ѵ�.
		auto it = std::lower_bound(scaleKeys.begin(), scaleKeys.end(), time, compKey3);

		lerpPercent = (time - (it-1)->first) / (it->first - (it-1)->first);
		XMVECTOR scale0 = XMLoadFloat3(&(it-1)->second);
		XMVECTOR scale1 = XMLoadFloat3(&it->second);
		scale = XMVectorLerp(scale0, scale1, lerpPercent);
	}

	if (quaternionKeys.empty())
		quaternion = XMQuaternionIdentity();
	else if (quaternionKeys.size() == 1 || time <= quaternionKeys[0].first)
		quaternion = XMLoadFloat4(&quaternionKeys[0].second);
	else if (time >= quaternionKeys.back().first)
		quaternion = XMLoadFloat4(&quaternionKeys.back().second);
	else
	{
		auto quatIt = std::lower_bound(quaternionKeys.begin(), quaternionKeys.end(), time, compKey4);
		lerpPercent = (time - (quatIt-1)->first) / (quatIt->first - (quatIt-1)->first);
		XMVECTOR quaternion0 = XMLoadFloat4(&(quatIt-1)->second);
		XMVECTOR quaternion1 = XMLoadFloat4(&quatIt->second);
		quaternion = XMQuaternionSlerp(quaternion0, quaternion1, lerpPercent);
	}

	if (translateKeys.empty())
		translate = XMVectorZero();
	else if (translateKeys.size() == 1 || time <= translateKeys[0].first)
		translate = XMLoadFloat3(&translateKeys[0].second);
	else if (time >= translateKeys.back().first)
		translate = XMLoadFloat3(&translateKeys.back().second);
	else
	{
		auto it = std::lower_bound(translateKeys.begin(), translateKeys.end(), time, compKey3);
		lerpPercent = (time - (it-1)->first) / (it->first - (it-1)->first);
		XMVECTOR translate0 = XMLoadFloat3(&(it-1)->second);
		XMVECTOR translate1 = XMLoadFloat3(&it->second);
		translate = XMVectorLerp(translate0, translate1, lerpPercent);
	}


	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&dest, XMMatrixAffineTransformation(
		scale, zero, quaternion, translate));
}

void BoneFrames::InitScales(std::vector<frameKey3> scales)
{
	scaleKeys.swap(scales);
	std::sort(scaleKeys.begin(), scaleKeys.end(), sortCompKey3);
}

void BoneFrames::InitQuaternions(std::vector<frameKey4> quaternions)
{
	quaternionKeys.swap(quaternions);
	std::sort(quaternionKeys.begin(), quaternionKeys.end(), sortCompKey4);
}

void BoneFrames::InitTranslations(std::vector<frameKey3> translations)
{
	translateKeys.swap(translations);
	std::sort(translateKeys.begin(), translateKeys.end(), sortCompKey3);
}



Animator::Animator(const Animator & other) : Animator()
{
	boneDatas = other.boneDatas;
	currClipName = other.currClipName;
	clips = other.clips;
}

Animator & Animator::operator=(const Animator & other)
{
	timePos = 0.0f;
	boneDatas = other.boneDatas;
	currClipName = other.currClipName;
	clips = other.clips;

	return *this;
}

void Animator::LoadAnimationClip(MyAnimationClip & clip)
{
	if (clips.find(clip.m_clipName) != clips.end())
		return;

	clips[clip.m_clipName] = clip;
}

void Animator::Update(float deltaTime)
{
	//���� ���������� �����ϴ� Animator�� ���
	//�ߺ� ������Ʈ ����
	if (AnimatedPerFrame)
		return;

#pragma region UpdateBody



	auto it = clips.find(currClipName);
	if (it == clips.end())
		return;

	MyAnimationClip& clip = it->second;

	timePos += deltaTime;

	//�ִϸ��̼��� �ִ� ����ð��� �ѱ�� ��� ó������ �ð� �ʱ�ȭ
	if (timePos > clip.duration)
		timePos = 0.0f;

	if (boneDatas.m_parentIndices.empty())
		return;

	int numBones = boneDatas.offsets.size();
	//�� ���뿡�� �θ����� ���� ���
	//std::vector<XMFLOAT4X4> toParents(numBones);
	std::vector<XMFLOAT4X4>&  toParents = boneDatas.toParents;
	toParents.resize(numBones);
	//�ִϸ��̼ǿ� ���� ��� ���
	clip.Interpolate(timePos, toParents);
	boneDatas.toRoots[0] = toParents[0];
	
	XMMATRIX toParent;
	XMMATRIX parentToRoot;
	XMMATRIX toRoot;
	XMMATRIX offset;
	int parentIdx;

	for (int i = 1; i < numBones; ++i)
	{
		
		toParent = XMLoadFloat4x4(&toParents[i]);
		
		//�ִϸ��̼��� ���� ���� ���� �⺻ �θ������ ������
		if (XMMatrixIsIdentity(toParent))
			toParent = XMLoadFloat4x4(&boneDatas.m_toParentMatrix[i]) * toParent;
		
		parentIdx = boneDatas.m_parentIndices[i];
		parentToRoot = XMLoadFloat4x4(&boneDatas.toRoots[parentIdx]);
		
		toRoot = XMMatrixMultiply(toParent, parentToRoot);
		XMStoreFloat4x4(&boneDatas.toRoots[i], toRoot);
	}
		
		
	for (int i = 0; i < numBones; ++i)
	{
		offset = XMLoadFloat4x4(&boneDatas.offsets[i]);
		toRoot = XMLoadFloat4x4(&boneDatas.toRoots[i]);
		
		XMStoreFloat4x4(&boneDatas.m_finalTransforms[i], XMMatrixMultiply(offset,toRoot));
	}
#pragma endregion

	//�̹� �����ӿ� ������Ʈ �Ϸ�
	AnimatedPerFrame = true;

}

bool Animator::DeleteAnimation(const std::string & clipName)
{
	return clips.erase(clipName);
}

bool Animator::ChangeClip(const std::string & clipName)
{
	if (clips.find(clipName) != clips.end())
	{
		currClipName = clipName;
		timePos = 0.0f;
		return true;
	}
	//ã�� Ŭ���� ���� ��
	return false;
}



//void AnimatorClip::InitHierarchy(std::map<std::wstring, int>& m_nodeNameIdx, AnimationClip & clip)
//{
//	duration = clip.duration;
//	ticksPerSecond = clip.ticksPerSecond;
//	m_clipName = clip.m_clipName;
//	m_bones.resize(m_nodeNameIdx.size(), nullptr);
//	int idx;
//	for (auto& bone : clip.m_bones)
//	{
//		auto it = m_nodeNameIdx.find(bone.m_boneName);
//		//�� �̸��� ��ġ�ϴ� ��尡 ������ �Ѿ.
//		if (it == m_nodeNameIdx.end())
//			continue;
//		idx = it->second;
//
//		m_bones[idx] = &bone;
//	}
//}

bool sortCompKey3(const frameKey3 & first, const frameKey3 & second)
{
	return first.first < second.first;
}

bool sortCompKey4(const frameKey4 & first, const frameKey4 & second)
{
	return first.first < second.first;
}

bool compKey3(const frameKey3 & first, float compVal)
{
	return first.first < compVal;
}

bool compKey4(const frameKey4 & first, float compVal)
{
	return first.first < compVal;
}

void MyAnimationClip::Interpolate(float time, std::vector<XMFLOAT4X4>& toParents)
{
	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	for (int i = 0; i < toParents.size(); ++i)
	{
		if (i >= m_bones.size())
		{
			toParents[i] = identity;
			continue;
		}
		m_bones[i].Interpolate(time, toParents[i]);
	}
}

void BoneDatas::SetOffsets(const std::vector<XMFLOAT4X4>& offsetVec)
{
	offsets = offsetVec;
}

void BoneDatas::SetParents(const std::vector<int>& parents)
{
	m_parentIndices = parents;
	toRoots.resize(parents.size());
	m_finalTransforms.resize(parents.size());
}

void BoneDatas::SetParentMatrix(const std::vector<XMFLOAT4X4>& parentMats)
{
	m_toParentMatrix.reserve(parentMats.size());
	m_toParentMatrix = parentMats;
}

void BoneDatas::SetBoneNameTransform(const std::map<std::wstring, int>& boneNameIdx, const std::wstring & boneName, std::shared_ptr<Transform> tr)
{
	SetBoneNameIdx(boneNameIdx);
	SetBoneTransform(boneName, tr);
}

void BoneDatas::SetBoneNameIdx(const std::map<std::wstring, int>& boneNameIdx)
{
	m_boneNameIdx = boneNameIdx;
	boneTransforms.resize(m_boneNameIdx.size());
}

void BoneDatas::SetBoneTransform(const std::wstring & boneName, std::shared_ptr<Transform> tr)
{
	auto it = m_boneNameIdx.find(boneName);
	if (it != m_boneNameIdx.end())
	{
		boneTransforms[it->second] = tr;
	}
}

bool BoneDatas::GetMatrixFromBoneName(XMFLOAT4X4 & dest, const std::wstring & boneName)
{
	auto it = m_boneNameIdx.find(boneName);
	if (it != m_boneNameIdx.end() && !toParents.empty())
	{
		dest = toParents[it->second];
		return true;
	}
	return false;
}

void BoneDatas::UpdateBoneTransforms()
{
	for (UINT i=0; i<boneTransforms.size(); ++i)
	{
		auto tr = boneTransforms[i];
		tr->m_world = toRoots[i];
	}
}
