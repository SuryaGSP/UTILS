#include <StringUtil.h>
#include <sstream>

namespace StringUtil
{
	char* strnstr(const char* s, const char* find, size_t slen)
	{
		char c, sc;
		size_t len;

		if ((c = *find++) != '\0')
		{
			len = strlen(find);
			do
			{
				do
				{
					if (slen-- < 1 || (sc = *s++) == '\0')
						return nullptr;
				}
				while (sc != c);
				if (len > slen)
					return nullptr;
			}
			while (strncmp(s, find, len) != 0);
			s--;
		}
		return ((char *)s);
	}

	char* strncpy(char* dst, const char* src, size_t len)
	{
		::strncpy(dst, src, len);
		dst[len - 1] = 0;
		return dst;
	}

	int IsEmptyString(const char* a)
	{
		if (a == nullptr
			|| strcmp(a, "") == 0
			|| strcmp(a, " ") == 0)
		{
			return 1;
		}
		return 0;
	}

	char* ConvertToLowerCase(char* src)
	{
		char* a = src;
		while (*a != '\0')
		{
			*a = tolower(*a);
			a++;
		}
		return src;
	}

	void ConvertToAscii(char* a)
	{
		while (*a != '\0')
		{
			int ch = *a;
			if (ch < 32 || ch > 126)
			{
				strcpy(a, "?");
				break;
			}
			a++;
		}
	}

	void ReplaceChar(char* a, char src, char rep)
	{
		while (*a != '\0')
		{
			if (*a == src)
			{
				*a = rep;
			}
			a++;
		}
	}

	char* ReplaceEscapeChar(char* line, char ch, char* res)
	{
		char* tempPkt;
		char *temp1, *nIdx;
		char* tok;
		int escNo = 0, n = 0;
		std::string str = (const char *)line;
		std::stringstream ss(str);

		tok = (char *)calloc(1, strlen((const char *)line) + 3);
		tempPkt = (char *)calloc(1, strlen((const char *)line) + 3);
		strcpy(tempPkt, (const char *)line);

		temp1 = tempPkt;
		while (getline(ss, str, ch))
		{
			sprintf(tok, "%s", str.c_str());

			escNo += sprintf(res + escNo, "%s", tok);
			if (ch == '%')
			{
				escNo += sprintf(res + escNo, "%s", "%%");
			}
			else if (ch == '\'')
			{
				escNo += sprintf(res + escNo, "%s", "''");
			}
			else
			{
				escNo += sprintf(res + escNo, "%s", "\\");
				escNo += sprintf(res + escNo, "%c", ch);
			}
			//firstTok++;
		}

		if (tempPkt != nullptr)
		{
			free(tempPkt);
		}

		//#ifdef WINDOWS
		res[escNo - 2] = '\0';

		return res;
	}

	int IsSubString(char* subStr, char* str)
	{
		int i, len1, len2;

		if (subStr == nullptr)
		{
			return 1;
		}
		if (str == nullptr)
		{
			return 0;
		}

		len1 = strlen(subStr);
		len2 = strlen(str);

		if (len1 > len2)
		{
			return 0;
		}

		for (i = 0; i < len2; i++)
		{
			if (str[i] == subStr[0] || (str[i] == subStr[0] - 32) || (str[i] == subStr[0] + 32))
			{
				if (_strnicmp(&str[i], subStr, len1) == 0)
				{
					return 1;
				}
			}
		}
		return 0;
	}

	int TrimAllNull(char* value, int len)
	{
		char* revValue = value + len - 1;
		while (!(*revValue))
		{
			--revValue;
			--len;
		}

		char* startValue = value;
		int normlen = len;
		while (normlen--)
		{
			if (!(*startValue))
			{
				*startValue = ' ';
			}
			++startValue;
		}
		return len;
	}

	//trim leading and trailing while spaces
	// length is excluding the null
	char* TrimWhiteSpace(char* str, int len)
	{
		char* src = str;
		while (isspace(static_cast<unsigned char>(*str)))
		{
			str++;
		}
		//All Empty spaces
		if (*str == 0)
		{
			*str = 0x00;
			return str;
		}
		char* end = src + len - 1;
		while (end > str && isspace(static_cast<unsigned char>(*end)))
		{
			end--;
		}
		*(end + 1) = 0x00;
		return str;
	}

	char* TrimWhiteSpace(char* str)
	{
		return TrimWhiteSpace(str, strlen(str));
	}

	std::string ReplaceString(std::string subject, const std::string& search,
	                          const std::string& replace)
	{
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos)
		{
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return subject;
	}

	std::string trim(const std::string& str,
	                 const std::string& whitespace)
	{
		const auto strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return "";

		const auto strEnd = str.find_last_not_of(whitespace);
		const auto strRange = strEnd - strBegin + 1;

		return str.substr(strBegin, strRange);
	}

	int parseBool(const char* b)
	{
		if (!_stricmp(b, "true")) return 1;
		if (!_stricmp(b, "false")) return 0;
		return -1;
	}

	int IsIpAddress(char* str)
	{
		int j, len1;
		char* indx;
		len1 = strlen(str);
		//255.255.255.255 ==> length is 15
		if (len1 > 15)
		{
			return 0;
		}

		j = 0;

		while (j < (len1 - 1))
		{
			if (str[j] == '.' && str[j + 1] == '.')
			{
				return 0;
			}
			j++;
		}

		indx = strchr(str, '.');

		if (indx == nullptr)
		{
			return 0;
		}

		if (indx - str > 3)
		{
			return 0;
		}

		for (j = 0; j < len1; j++)
		{
			if ((str[j] >= '0' && str[j] <= '9') || (str[j] == '.'))
			{
				continue;
			}
			return 0;
		}
		return 1;
	}

	std::string Quoted(std::string a)
	{
		return "\"" + a + "\"";
	}

	void CopyCharOrDefault(char* destination, char* source, const char* defaultValue)
	{
		if (source != nullptr)
		{
			if (strlen(source) > 0)
			{
				strcpy(destination, source);
			}
			else
			{
				strcpy(destination, defaultValue);
			}
		}
		else
		{
			strcpy(destination, defaultValue);
		}
	}

	void CopyCharOrDefaultWithLength(char* destination, char* source, const char* defaultValue, int maxLength)
	{
		if (source != nullptr)
		{
			if (strlen(source) > 0)
			{
				strncpy(destination, source, maxLength);
			}
			else
			{
				strncpy(destination, defaultValue, strlen(defaultValue) + 1);
			}
		}
		else
		{
			strncpy(destination, defaultValue, strlen(defaultValue) + 1);
		}
	}


	std::string RemoveNewlineFromStr(std::string str)
	{
		std::string::size_type pos = 0;
		while ((pos = str.find("\n", pos)) != std::string::npos)
		{
			str.erase(pos, 1);
		}

		return str;
	}

	std::string RemoveTrailingNewlineFromStr(std::string str)
	{
		std::string trimmed;
		std::string::size_type pos = 0;

		pos = str.find_last_not_of('\n');

		if (pos != std::string::npos)
		{
			trimmed = str.substr(0, pos + 1);
		}

		return trimmed;
	}

	std::string RemoveTrailingSpaceFromStr(std::string str)
	{
		std::string trimmed;
		std::string::size_type pos = 0;

		pos = str.find_last_not_of(' ');

		if (pos != std::string::npos)
		{
			trimmed = str.substr(0, pos + 1);
		}

		return trimmed;
	}

	bool IsUnreserved(unsigned char in)
	{
		switch (in)
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case 'a': case 'b': case 'c': case 'd': case 'e':
		case 'f': case 'g': case 'h': case 'i': case 'j':
		case 'k': case 'l': case 'm': case 'n': case 'o':
		case 'p': case 'q': case 'r': case 's': case 't':
		case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E':
		case 'F': case 'G': case 'H': case 'I': case 'J':
		case 'K': case 'L': case 'M': case 'N': case 'O':
		case 'P': case 'Q': case 'R': case 'S': case 'T':
		case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case '-': case '.': case '_': case '~':
			return true;
		default:
			break;
		}
		return false;
	}

	std::string EncodeURL(std::string input)
	{
		std::stringstream output;

		for (auto in : input)
		{
			if (IsUnreserved(in))
			{
				output << in;
			}
			else
			{
				output << '%';
				output << std::hex << int(in);
			}
		}
		return output.str();
	}

	bool IsSubString(const char* subStr, const char* str)
	{
		int i, len1, len2;

		if (subStr == nullptr)
		{
			return true;
		}
		if (str == nullptr)
		{
			return false;
		}

		len1 = strlen(subStr);
		len2 = strlen(str);

		if (len1 > len2)
		{
			return false;
		}

		for (i = 0; i < len2; i++)
		{
			if (str[i] == subStr[0] || (str[i] == subStr[0] - 32) || (str[i] == subStr[0] + 32))
			{
#ifdef WINDOWS
				if (_strnicmp(&str[i], subStr, len1) == 0)
				{
					return true;
				}
#else
				if (strncasecmp(&str[i], subStr, len1) == 0) {
					return true;
				}
#endif
			}
		}
		return false;
	}

	void MakeUniformPath(std::string &input)
	{
		std::stringstream ss;
		size_t pos = std::string::npos;
		do
		{
			pos = input.find("\\\\");
			int inputlen = input.length();

			if (pos != std::string::npos)
			{
				ss << input.substr(0, pos);
				if (pos + 2 != inputlen)
				{
					ss << input.substr(pos + 1, inputlen);
				}
				input = ss.str();
			}

			ss.str(std::string());

		} while (pos != std::string::npos);

	}
}
