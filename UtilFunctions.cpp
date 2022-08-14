#include "UtilFunctions.h"

std::wstring GetProjectDirectory()
{
	return std::wstring(fs::current_path().c_str()) + L"/";
}

std::wstring ConvertWSTR(const std::string& str)
{
	USES_CONVERSION;
	LPCWSTR wstr;
	wstr = A2W(str.c_str());

	return wstr;
}

std::string ConvertSTR(const std::wstring & wstr)
{
	USES_CONVERSION;
	LPCSTR str;
	str = W2A(wstr.c_str());

	return str;
}

std::wstring ExtractTitle(const std::wstring & FilePath)
{
	//diffuse, normal map들 이름 설정
	int rFirstdot = FilePath.rfind(L'.');
	if (rFirstdot != -1)
	{
		int rFirstSlash = FilePath.rfind(L'\\', rFirstdot);
		return FilePath.substr(rFirstSlash + 1);
	}
	return std::wstring(L"NULL");
}

std::string ExtractTitle(const std::string & FilePath)
{
	//diffuse, normal map들 이름 설정
	int rFirstdot = FilePath.rfind(L'.');
	if (rFirstdot != -1)
	{
		int rFirstSlash = FilePath.rfind(L'\\', rFirstdot);
		return FilePath.substr(rFirstSlash + 1);
	}
	return std::string("NULL");
}

XNA::AxisAlignedBox ConvertAABBtypeToCenter(const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector)
{
	//AABB의 중점계산
	XMVECTOR MaxVec = XMLoadFloat3(&MaxVector);
	XMVECTOR MinVec = XMLoadFloat3(&MinVector);
	XMVECTOR CenterVector = 0.5*(MaxVec + MinVec);

	//AABB의 중점에서 각 면까지의 거리 계산
	XMVECTOR Extents = 0.5*(MaxVec - MinVec);

	XNA::AxisAlignedBox aabb;
	XMStoreFloat3(&aabb.Center, CenterVector);
	XMStoreFloat3(&aabb.Extents, Extents);

	return aabb;
}

XNA::AxisAlignedBox & ConvertAABBtypeToCenter(XNA::AxisAlignedBox & Dest, const XMFLOAT3& MaxVector, const XMFLOAT3& MinVector)
{
	//AABB의 중점계산
	XMVECTOR MaxVec = XMLoadFloat3(&MaxVector);
	XMVECTOR MinVec = XMLoadFloat3(&MinVector);
	XMVECTOR CenterVector = 0.5*(MaxVec + MinVec);

	//AABB의 중점에서 각 면까지의 거리 계산
	XMVECTOR Extents = 0.5*(MaxVec - MinVec);
	
	//매개변수로 받은 AABB의 값을 수정
	XMStoreFloat3(&Dest.Center, CenterVector);
	XMStoreFloat3(&Dest.Extents, Extents);

	return Dest;
}

