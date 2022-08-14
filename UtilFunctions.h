#pragma once
#include <string>
#include <atlbase.h>
#include <xnamath.h>
#include <BufferResource.h>
#include <xnacollision.h>

#include <filesystem>

namespace fs = std::filesystem;


std::wstring GetProjectDirectory();
//str을 wstr로 변환
std::wstring ConvertWSTR(const std::string& str);
//wstr을 str로 변환
std::string ConvertSTR(const std::wstring& str);
//파일경로에서 파일명만 추출
std::wstring ExtractTitle(const std::wstring & FilePath);
std::string ExtractTitle(const std::string & FilePath);


//최대,최소 벡터로 표현된 AABB를 중점,거리 표현으로 변환
XNA::AxisAlignedBox ConvertAABBtypeToCenter(const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);
XNA::AxisAlignedBox& ConvertAABBtypeToCenter(XNA::AxisAlignedBox& Dest, const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);


