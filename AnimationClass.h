#pragma once
#include <d3dUtil.h>
#include <algorithm>
#include <map>
#include <GameTimer.h>

//시간, 해당 키 프레임의 벡터
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
class MyAnimationClip
{
public:
	double duration;
	std::string m_clipName; //animation clip의 이름
	std::vector<BoneFrames> m_bones;

	void Interpolate(float time, std::vector<XMFLOAT4X4>& toParents);
};



struct BoneDatas
{
	//Offset 행렬
	std::vector<XMFLOAT4X4> offsets;
	//Bone 계층구조
	std::vector<int> m_parentIndices;
	//각 뼈대에서 Root로 가는 행렬
	std::vector<XMFLOAT4X4> toRoots;
	//각 뼈대의 최종 변환
	std::vector<XMFLOAT4X4> m_finalTransforms;
	//부모 뼈대로의 변환
	std::vector<XMFLOAT4X4> m_toParentMatrix;

	void SetOffsets(const std::vector<XMFLOAT4X4>& offsetVec);
	void SetParents(const std::vector<int>& parents);
	void SetParentMatrix(const std::vector<XMFLOAT4X4>& parentMats);
};


class Animator
{
public:
	Animator() : timePos(0.0f)  {}
	Animator(const Animator& other);
	Animator& operator=(const Animator& other);
	//애니메이터에서 참조하는 뼈 구조
	BoneDatas boneDatas;
		
	//현재 실행하는 클립이름
	std::string currClipName;
	//현재 실행중인 시간위치
	float timePos;
	//현재 애니메이터에서 실행할 수 있는 클립들
	std::map<std::string, MyAnimationClip> clips;
	
	
public:
	void SetBoneDatas(BoneDatas& data) { boneDatas = data; }
	void LoadAnimationClip(MyAnimationClip& clip);
	void Update(float deltaTime);

public:
	void ChangeClip(const std::string& clipName);
};