#pragma once
#include <string>
#include <atlbase.h>
#include <xnamath.h>
#include <BufferResource.h>
#include <xnacollision.h>


//str�� wstr�� ��ȯ
std::wstring ConvertWSTR(const std::string& str);

//�ִ�,�ּ� ���ͷ� ǥ���� AABB�� ����,�Ÿ� ǥ������ ��ȯ
XNA::AxisAlignedBox ConvertAABBtypeToCenter(const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);
XNA::AxisAlignedBox& ConvertAABBtypeToCenter(XNA::AxisAlignedBox& Dest, const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);
