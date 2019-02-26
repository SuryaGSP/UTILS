#pragma once
#ifdef WINDOWS
#include <ELAWindows.h>
#else
#include <pthread.h>
#include <sys/types.h>
#endif
#include<string>

namespace EncodingUtil
{
#ifdef WINDOWS
	std::string WideStringToMultibyte(LPWSTR utf16string, unsigned int codepage = CP_UTF8);
	std::wstring MultibyteToWideString(const char* mbcs);
	std::wstring MultibyteToWideString(std::string mbcs);
#endif
	int HexToDec(std::string);
	int StringToInt(std::string);
	std::string IntToStr(int);
	std::string ULLToStr(unsigned long long);
	long long StrToLL(std::string);
}
