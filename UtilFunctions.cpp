#include "UtilFunctions.h"

std::wstring ConvertWSTR(std::string str)
{
	USES_CONVERSION;
	LPCWSTR wstr;
	wstr = A2W(str.c_str());

	return wstr;
}
