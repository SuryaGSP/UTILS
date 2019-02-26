#include <EncodingUtil.h>
#include <sstream>

namespace EncodingUtil
{
#ifdef WINDOWS
	std::string WideStringToMultibyte(LPWSTR utf16string, unsigned int codepage)
	{
		if (utf16string == nullptr)
			return std::string();
		int cw = lstrlenW(utf16string);
		if (cw == 0)
		{
			return std::string();
		}
		int cc = WideCharToMultiByte(codepage, 0, utf16string, cw, nullptr, 0, nullptr, nullptr);
		if (cc == 0)
			return std::string();
		CHAR* psz = static_cast<char *>(malloc(cc + 1));
		cc = WideCharToMultiByte(codepage, 0, utf16string, cw, psz, cc, nullptr, nullptr);
		if (cc == 0)
		{
			free(psz);
			return std::string();
		}
		psz[cc] = '\0';
		std::string multibyteString(psz);
		free(psz);
		return multibyteString;
	}

	std::wstring MultibyteToWideString(const char* mbcs)
	{
		if (mbcs == nullptr)
			return std::wstring();
		int ca = strlen(mbcs);
		if (ca == 0)
		{
			return std::wstring();
		}
		wchar_t* wString = nullptr;
		int size = MultiByteToWideChar(CP_UTF8, 0, mbcs, -1, wString, 0);
		if (size)
		{
			wString = static_cast<wchar_t *>(calloc(size, sizeof(wchar_t) + 1));
			int z = MultiByteToWideChar(CP_UTF8, 0, mbcs, -1, wString, size);
			if (!z)
			{
				free(wString);
				return std::wstring();
			}
		}
		std::wstring wideString(wString);
		if (wString)
		{
			free(wString);
		}
		return wideString;
	}

	std::wstring MultibyteToWideString(std::string mbcs)
	{
		return MultibyteToWideString(mbcs.c_str());
	}
#endif
	int HexToDec(std::string hexadecimal)
	{
		std::stringstream stream;
		int decimal = 0;
		stream << hexadecimal;
		stream >> std::hex >> decimal;
		return decimal;
	}

	int StringToInt(std::string str)
	{
		std::stringstream stream;
		int number = 0;
		stream << str;
		stream >> number;
		return number;
	}

	std::string IntToStr(int val)
	{
		std::stringstream stream;
		stream << val;
		return stream.str();
	}

	std::string ULLToStr(unsigned long long val) 
	{
		std::stringstream stream;
		stream << val;
		return stream.str();
	}

	long long StrToLL(std::string str)
	{
		std::stringstream stream;
		long long number = 0;
		stream << str;
		stream >> number;
		return number;
	}
}
