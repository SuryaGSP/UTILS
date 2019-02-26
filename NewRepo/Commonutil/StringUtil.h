#pragma once
#include <string>

#ifdef UNIX
#include <cstring>
#define stricmp(a,b) strcasecmp(a,b)
#define strnicmp(a,b,c) strncasecmp(a,b,c)
#endif

namespace StringUtil
{
	char* strnstr(const char* s, const char* find, size_t slen);
	char* ConvertToLowerCase(char* a);
	void ConvertToAscii(char* ptr);
	void ReplaceChar(char* a, char src, char rep);
	int IsEmptyString(char const* a);
	int parseBool(const char* a);
	char* strncpy(char* dst, const char* src, size_t len);
	int IsIpAddress(char* str);
	std::string ReplaceString(std::string subject, const std::string& search,
	                          const std::string& replace);
	std::string trim(const std::string& str,
	                 const std::string& whitespace = " \t");
	int IsSubString(char* subStr, char* str);
	int TrimAllNull(char* value, int len);
	char* TrimWhiteSpace(char* str);
	char* TrimWhiteSpace(char* str, int len);
	void CopyCharOrDefault(char* destination, char* source, const char* defaultValue);
	void CopyCharOrDefaultWithLength(char* destination, char* source, const char* defaultValue, int length);
	char* ReplaceEscapeChar(char* line, char ch, char* res);
	std::string Quoted(std::string a);
	std::string RemoveNewlineFromStr(std::string str);
	std::string RemoveTrailingNewlineFromStr(std::string str);
	std::string RemoveTrailingSpaceFromStr(std::string str);
	bool IsUnreserved(unsigned char in);
	std::string EncodeURL(std::string);
	bool IsSubString(const char* subStr, const char* str); 
	void MakeUniformPath(std::string &input);
}
