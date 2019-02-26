#include <CommonUtil.h>
#ifndef WINDOWS
#include <dirent.h>
#include <sys/stat.h>
#endif

#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>

#include <StringUtil.h>

#ifdef UNIX
	void GetLocalTime(time_t *localtime)
	{
		*localtime = time(NULL);
	}

	void InterlockedIncrement(long *var)
	{
		__sync_add_and_fetch(var,1);
	}

	void InterlockedExchange(long *var, long value)
	{
		__sync_lock_test_and_set(var, value);
	}

	int TryEnterCriticalSection(pthread_mutex_t* lock)
	{
		bool stat;
		stat = pthread_mutex_trylock(lock);
		return stat == 0 ? 1 : 0;
	}

void DeleteCriticalSection(pthread_mutex_t* lock)
{
	bool stat;
	stat = pthread_mutex_destroy(lock);
}
#endif

namespace CommonUtil
{
	int IsBitSet(int& number, int bit)
	{
		return (number >> bit) & 1;
	}

	int SetBit(int& number, int bit)
	{
		return number |= 1 << bit;
	}

	int ClearBit(int& number, int bit)
	{
		return number &= ~(1 << bit);
	}

	bool IsDigit(std::string numbers)
	{
		return all_of(numbers.begin(), numbers.end(), ::isdigit);
	}

	std::string DecimalToBinary(char decimal)
	{
		std::string binary;
		int dec = decimal - '0';
		while (dec > 0)
		{
			binary += std::to_string(dec % 2);
			dec = dec / 2;
		}
		reverse(binary.begin(), binary.end());
		return binary;
	}

	std::string OctalToBinary(char octal)
	{
		std::string binary;
		int oct = octal - '0';
		if (oct == 0)
		{
			binary = "000";
			return binary;
		}
		if (oct > 7)
		{
			binary = "111";
			return binary;
		}
		while (oct > 0)
		{
			binary += std::to_string(oct % 2);
			oct = oct / 2;
		}
		std::reverse(binary.begin(), binary.end());
		return binary;
	}

	std::vector<std::string> GetVectorFromCSV(std::string& csv)
	{
		std::string token;
		std::vector<std::string> tokens;
		std::istringstream split(csv);

		while (getline(split, token, ','))
		{
			auto clean_token = StringUtil::trim(token);
			tokens.push_back(clean_token);
		}

		return tokens;
	}

	std::string GetProperty(std::map<std::string, std::string> confs, std::string key)
	{
		if (confs.count(key))
		{
			return confs[key];
		}
		return "";
	}

#ifdef WINDOWS
	std::string WmiErrorValToHexString(HRESULT hResult)
	{
		std::stringstream ss;
		ss << std::showbase << std::uppercase << std::hex << hResult;
		std::string result = ss.str();
		return result;
	}

#endif
};