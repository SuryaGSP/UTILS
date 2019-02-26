#pragma once
#ifdef WINDOWS
#include <ELAWindows.h>
#else
	#include <pthread.h>
	#include <sys/types.h>
#endif
#include <vector>
#include <string>
#include <map>

// Common Unix to Windows Translation (!) defines's
#ifdef UNIX
	#define stricmp(a,b) strcasecmp(a,b)
	#define strnicmp(a,b,c) strncasecmp(a,b,c)
	#define SYSTEMTIME time_t
	#define CRITICAL_SECTION pthread_mutex_t
	#define EnterCriticalSection(lock) pthread_mutex_lock(lock)
	#define LeaveCriticalSection(lock) pthread_mutex_unlock(lock)
	#define InitializeCriticalSection(lock) pthread_mutex_init(lock,NULL)	
	#define GetCurrentThreadId() (long int)syscall(224)
	void GetLocalTime(time_t *localtime);
	void InterlockedIncrement(long *var);
	void InterlockedExchange(long *var, long val);
	void DeleteCriticalSection(pthread_mutex_t* lock);
	int TryEnterCriticalSection(pthread_mutex_t* lock);
#endif

namespace CommonUtil
{
	int ClearBit(int& number, int bit);
	int SetBit(int& number, int bit);
	int IsBitSet(int& number, int bit);
	bool IsDigit(std::string numbers);
	std::string DecimalToBinary(char decimal);
	std::string OctalToBinary(char octal);
	std::vector<std::string> GetVectorFromCSV(std::string& csv);
	std::string GetProperty(std::map<std::string, std::string> confs, std::string key);

	template <typename genericType>
	bool EntryExistsInVector(std::vector<genericType> container, genericType value)
	{
		for (genericType val : container)
		{
			if (val == value)
			{
				return true;
			}
		}
		return false;
	}

	template <typename genericType>
	std::pair<bool, genericType> FindValueInMap(std::map<std::string, genericType> storage, std::string key)
	{
		auto it = storage.find(key);
		genericType nullVal;
		if (it != storage.end())
		{
			return std::make_pair(true, it->second);
		}
		return std::make_pair(false, nullVal);
	}
#ifdef WINDOWS
	std::string WmiErrorValToHexString(HRESULT hResult);
#endif
};

template <typename generic>
struct caseInsensitiveFind
{
	caseInsensitiveFind(std::string in_logtype)
	{
		logtype = in_logtype;
	}

	bool operator()(const std::pair<std::string, generic>& in_map) const
	{
		return !_stricmp(in_map.first.c_str(), logtype.c_str());
	}

private:
	std::string logtype;
};
