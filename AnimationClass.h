#pragma once
#include <d3dUtil.h>
#include <algorithm>
#include <map>

typedef std::pair<float, XMFLOAT3> frameKey3;
typedef std::pair<float, XMFLOAT4> frameKey4;

bool sortCompKey3(const frameKey3& first, const frameKey3& second);
bool sortCompKey4(const frameKey4& first, const frameKey4& second);
bool compKey3(const frameKey3& first, float compVal);
bool compKey4(const frameKey4& first, float compVal);

//뼈 하나에 대해 저장된 KeyFrame들
//애니메이션으로 보간된 행렬은 부모변환 행렬로 사용됨.
class BoneFrames
{
public:
	std::wstring m_boneName;
	BoneFrames(const std::wstring& name) : m_boneName(name) {}
private:
	std::vector<frameKey3> scaleKeys;
	std::vector<frameKey4> quaternionKeys;
	std::vector<frameKey3> translateKeys;
public:
	void Interpolate(float time, XMFLOAT4X4& dest);
	void InitScales(std::vector<frameKey3> scales);
	void InitQuaternions(std::vector<frameKey4> quaternions);
	void InitTranslations(std::vector<frameKey3> translations);
};

//AnimationManager에 저장용 클래스
class AnimationClip
{
public:
	double duration;
	double ticksPerSecond;
	std::string m_clipName; //animation clip의 이름
	std::vector<BoneFrames> m_bones;
	
};

typedef std::vector<BoneFrames*> AnimatorBones;

class AnimatorClip
{
public:
	AnimatorClip(){ XMStoreFloat4x4(&identity, XMMatrixIdentity()); }
public:
	double duration;
	double ticksPerSecond;
	std::string m_clipName;
	AnimatorBones m_bones;
	XMFLOAT4X4 identity;
public:
	void Interpolate(float time, std::vector<XMFLOAT4X4>& toParents);
	//현재 이 클립을 사용하는 노드 구조에 맞춰 m_bones를 초기화
	void InitHierarchy(std::map<std::wstring, int>& m_nodeNameIdx, AnimationClip& clip);
};



class Animator
{
public:
	Animator() {}
	std::map<std::wstring, int>& m_nodeNameIdx;
	std::vector<int>& m_parentIndices;
	//Root로 가는 행렬
	std::vector<XMFLOAT4X4> toRoots; 
	//Offset 행렬
	std::vector<XMFLOAT4X4> offsets;
	//현재 애니메이터에서 실행할 수 있는 클립들
	std::map<std::string, AnimatorClip> clips;
	//현재 실행하는 클립이름
	std::string currClipName;
public:
	void AddNode(std::wstring* parentName, std::wstring* childName, XMFLOAT4X4& offsetMat);
	void LoadAnimationClip(AnimationClip& clip);
	void Update(float time);
};