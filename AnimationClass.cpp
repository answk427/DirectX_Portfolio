#include "AnimationClass.h"

void BoneFrames::Interpolate(float time, XMFLOAT4X4& dest)
{
	//time보다 크거나 같은 키프레임을 찾아 보간한다.
	auto it = std::lower_bound(scaleKeys.begin(), scaleKeys.end(), time, compKey3);

	float lerpPercent = (time - it->first) / ((it + 1)->first - it->first);
	XMVECTOR scale0 = XMLoadFloat3(&it->second);
	XMVECTOR scale1 = XMLoadFloat3(&(it + 1)->second);
	XMVECTOR scale = XMVectorLerp(scale0, scale1, lerpPercent);

	auto quatIt = std::lower_bound(quaternionKeys.begin(), quaternionKeys.end(), time, compKey4);
	lerpPercent = (time - quatIt->first) / ((quatIt + 1)->first - quatIt->first);
	XMVECTOR quaternion0 = XMLoadFloat4(&quatIt->second);
	XMVECTOR quaternion1 = XMLoadFloat4(&(quatIt + 1)->second);
	XMVECTOR quaternion = XMQuaternionSlerp(quaternion0, quaternion1, lerpPercent);


	XMMatrixRotationQuaternion(quaternion);
	it = std::lower_bound(translateKeys.begin(), translateKeys.end(), time, compKey3);
	lerpPercent = (time - it->first) / ((it + 1)->first - it->first);
	XMVECTOR translate0 = XMLoadFloat3(&it->second);
	XMVECTOR translate1 = XMLoadFloat3(&(it + 1)->second);
	XMVECTOR translate = XMVectorLerp(translate0, translate1, lerpPercent);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&dest, XMMatrixAffineTransformation(
		scale, zero, quaternion, translate));
}

void BoneFrames::InitScales(std::vector<frameKey3> scales)
{
	scaleKeys = scales;
	std::sort(scaleKeys.begin(), scaleKeys.end(), sortCompKey3);
}

void BoneFrames::InitQuaternions(std::vector<frameKey4> quaternions)
{
	quaternionKeys = quaternions;
	std::sort(quaternionKeys.begin(), quaternionKeys.end(), sortCompKey4);
}

void BoneFrames::InitTranslations(std::vector<frameKey3> translations)
{
	translateKeys = translations;
	std::sort(translateKeys.begin(), translateKeys.end(), sortCompKey3);
}



Animator::Animator(std::map<std::wstring, int>& nodeNameIdx, std::vector<int>& parentIndices) :
	m_nodeNameIdx(nodeNameIdx), m_parentIndices(parentIndices)
{
	toRoots.resize(m_parentIndices.size());

}

void Animator::AddNode(std::wstring * parentName, std::wstring * childName, XMFLOAT4X4 & offsetMat)
{
	if (childName == nullptr)
		return;
	//rootNode 삽입일 경우 부모 값은 -1
	if (parentName == nullptr)
		m_parentIndices.push_back(-1);
	else
		m_parentIndices.push_back(m_nodeNameIdx[*parentName]);

	m_nodeNameIdx[*childName] = m_parentIndices.size() - 1;
	nodeNameIdx[*nodeName] = parentIndices.size() - 1;

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());


	toParents.push_back(transformPtr);
	offsets.push_back(identity);
	finalTransforms.push_back(identity);
}

void Animator::LoadAnimationClip(AnimationClip & clip)
{
	if (clips.find(clip.m_clipName) != clips.end())
		return;

	clips.insert({ clip.m_clipName, AnimatorClip() });

	//현재 노드구조만큼 BoneFrame 사이즈를 할당
	clips[clip.m_clipName].InitHierarchy(m_nodeNameIdx, clip);
}

void Animator::Update(float time)
{
	auto clip = clips[currClipName];
	std::vector<XMFLOAT4X4> toParents(m_parentIndices.size());
	//애니메이션에 따른 행렬 계산
	clip.Interpolate(time, toParents);
	toRoots[0] = toParents[0];
	XMMATRIX toParent;
	XMMATRIX toRoot;
	for (int i = 1; i < toRoots.size(); ++i)
	{
		toParent = XMLoadFloat4x4(&toParents[i]);
		toRoot = XMLoadFloat4x4(&toRoots[i - 1]);
		XMStoreFloat4x4(&toRoots[i], XMMatrixMultiply(toParent, toRoot));
	}
}



void AnimatorClip::Interpolate(float time, std::vector<XMFLOAT4X4>& toParents)
{
		
	for (int i = 0; i < toParents.size(); ++i)
	{
		if (m_bones[i] == nullptr)
			toParents[i] = identity;
		else
			m_bones[i]->Interpolate(time, toParents[i]);
	}

}

void AnimatorClip::InitHierarchy(std::map<std::wstring, int>& m_nodeNameIdx, AnimationClip & clip)
{
	duration = clip.duration;
	ticksPerSecond = clip.ticksPerSecond;
	m_clipName = clip.m_clipName;
	m_bones.resize(m_nodeNameIdx.size(), nullptr);
	int idx;
	for (auto& bone : clip.m_bones)
	{
		auto it = m_nodeNameIdx.find(bone.m_boneName);
		//뼈 이름과 일치하는 노드가 없으면 넘어감.
		if (it == m_nodeNameIdx.end())
			continue;
		idx = it->second;

		m_bones[idx] = &bone;
	}
}

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
	return first.first<compVal;
}
