#pragma once
#include <vector>
#include <string>
#include <functional>

#ifdef UNIX
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#else
#include <Windows.h>
#endif
namespace SystemUtil
{
	void ForEachMatchingFile(std::function<void(const char*, _WIN32_FIND_DATAA)> callback, const char* filename, std::string pattern);
	void RemoveFile(const char* path, const char* filename);
	std::string ExecuteSystemCommand(const char* cmd);
	long long GetFileSizeInBytes(char* fileName);
#ifdef WINDOWS
	void CreateWinProcess(std::string processname, char* uploaderArg);
	DWORD IsWin32ProcessRunning(const char* processname);
#endif
}
