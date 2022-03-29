#pragma once
#include <string>
#include <atlbase.h>
#include <xnamath.h>
#include <BufferResource.h>
#include <xnacollision.h>


//str을 wstr로 변환
std::wstring ConvertWSTR(const std::string& str);

//최대,최소 벡터로 표현된 AABB를 중점,거리 표현으로 변환
XNA::AxisAlignedBox ConvertAABBtypeToCenter(const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);
XNA::AxisAlignedBox& ConvertAABBtypeToCenter(XNA::AxisAlignedBox& Dest, const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);
