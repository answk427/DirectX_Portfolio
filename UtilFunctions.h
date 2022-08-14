#pragma once
#include <string>
#include <atlbase.h>
#include <xnamath.h>
#include <BufferResource.h>
#include <xnacollision.h>

#include <filesystem>

namespace fs = std::filesystem;


std::wstring GetProjectDirectory();
//str�� wstr�� ��ȯ
std::wstring ConvertWSTR(const std::string& str);
//wstr�� str�� ��ȯ
std::string ConvertSTR(const std::wstring& str);
//���ϰ�ο��� ���ϸ� ����
std::wstring ExtractTitle(const std::wstring & FilePath);
std::string ExtractTitle(const std::string & FilePath);


//�ִ�,�ּ� ���ͷ� ǥ���� AABB�� ����,�Ÿ� ǥ������ ��ȯ
XNA::AxisAlignedBox ConvertAABBtypeToCenter(const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);
XNA::AxisAlignedBox& ConvertAABBtypeToCenter(XNA::AxisAlignedBox& Dest, const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector);


