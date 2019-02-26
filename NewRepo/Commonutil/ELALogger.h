#pragma once
#pragma warning(disable:4996)
/* Hanlded path, directory and name default values
 * To Hnalde support for console logging
 */
#include <string>
#include <sstream>
#include <mutex>
#include <vector>
#include <iostream>
#ifdef WINDOWS
#include <Windows.h>
#include<errno.h>
#else
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

namespace LoggerUtil
{
	inline std::string WideCharToMultiByte(std::wstring src)
	{
		int len = src.length();
		if (len == 0)
		{
			return nullptr;
		}
		std::vector<char> buffer(len + 1);
		char* rawbuffer = reinterpret_cast<char*>(buffer.data());
		int result = std::wcstombs(rawbuffer, src.c_str(), len);
		buffer[len] = '\0';
		return rawbuffer;
	}

	//getthreadid
	inline int GetThreadID()
	{
#ifdef WINDOWS
		return GetCurrentThreadId();
#else
		return gettid();
#endif
	}

	//getfilesize
	inline long long GetFileDiskSize(std::string filename)
	{
		long long fileSize = 0;
#ifdef WINDOWS
#ifdef LOGAGENT
		HANDLE  hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
		HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
		                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
#endif
		if (hFile != INVALID_HANDLE_VALUE)
		{
			fileSize = GetFileSize(hFile, nullptr);
			CloseHandle(hFile);
		}
#else
		struct stat lf;
		if (stat(filename.c_str(), &lf) == 0)
		{
			fileSize = lf.st_size;
		}
#endif
		return fileSize;
	}

	inline std::string GetFileName(std::string& filename)
	{
		std::size_t found = filename.find_last_of("/\\");
		if (found != std::string::npos)
		{
			return filename.substr(found + 1);
		}
		return filename;
	}

	inline std::string GetFileNameWithOutExtension(std::string& filename)
	{
		std::string file = GetFileName(filename);
		std::size_t found = file.find_last_of(".");
		if (found != std::string::npos)
		{
			return file.substr(0, found);
		}
		return file;
	}

	inline std::string GetExtension(std::string& filename)
	{
		std::string file = GetFileName(filename);
		std::size_t found = file.find_last_of(".");
		if (found != std::string::npos)
		{
			return file.substr(found + 1);
		}
		return "";
	}

	inline std::string GetParent(std::string& filename)
	{
		std::size_t found = filename.find_last_of("/\\");
		if (found != std::string::npos)
		{
			return filename.substr(0, found);
		}
		return "";
	}

	//getmodulename
	inline std::string GetModuleName()
	{
#ifdef WINDOWS
#ifdef LOGAGENT
		WCHAR path[MAX_PATH];
		GetModuleFileNameW(NULL, path, MAX_PATH);
        std::string tempStringPathName = WideCharToMultiByte(path);
		return GetFileNameWithOutExtension(tempStringPathName);
#else
		TCHAR path[MAX_PATH];
		GetModuleFileName(nullptr, path, MAX_PATH);
        std::string tempStringPathName = std::string(path);
		return GetFileNameWithOutExtension(tempStringPathName);
#endif
#else
		char result[256];
		ssize_t count = readlink("/proc/self/exe", result, 256);
		std::string path;
		if (count != -1)
		{
			path = dirname(result);
		}
		return path;
#endif
	}
}

enum LogLevel
{
	none = 0,
	fatal = 1,
	error = 2,
	warning = 3,
	info = 4,
	debug = 5,
	verbose = 6
};

class MessageBuilder
{
public:
#   define ELA_SIMPLE_LOG(LOG_TYPE)\
		inline MessageBuilder& operator<<(LOG_TYPE msg) {\
			stream << msg; \
			return *this; \
		}

	MessageBuilder& operator<<(const std::string& msg)
	{
		return operator<<(msg.c_str());
	}

#ifdef WINDOWS
	MessageBuilder& operator<<(const std::wstring& msg)
	{
		std::string mbcsString(LoggerUtil::WideCharToMultiByte((LPWSTR)msg.c_str()));
		return operator<<(mbcsString.c_str());
	}

	MessageBuilder& operator<<(LPWSTR msg)
	{
		std::string mbcsString(LoggerUtil::WideCharToMultiByte(msg));
		return operator<<(mbcsString.c_str());
	}

	MessageBuilder& operator<<(LPCWSTR msg)
	{
		std::string mbcsString(LoggerUtil::WideCharToMultiByte((LPWSTR)msg));
		return operator<<(mbcsString.c_str());
	}
#endif

	std::stringstream& GetString()
	{
		return stream;
	}

	void Clear()
	{
		stream.str(std::string());
		stream.clear();
	}

	ELA_SIMPLE_LOG(char)
	ELA_SIMPLE_LOG(bool)
	ELA_SIMPLE_LOG(signed short)
	ELA_SIMPLE_LOG(unsigned short)
	ELA_SIMPLE_LOG(signed int)
	ELA_SIMPLE_LOG(unsigned int)
	ELA_SIMPLE_LOG(signed long)
	ELA_SIMPLE_LOG(unsigned long)
	ELA_SIMPLE_LOG(float)
	ELA_SIMPLE_LOG(double)
	ELA_SIMPLE_LOG(char*)
	ELA_SIMPLE_LOG(const char*)
	ELA_SIMPLE_LOG(const void*)
	ELA_SIMPLE_LOG(long double)
	ELA_SIMPLE_LOG(unsigned long long)
	ELA_SIMPLE_LOG(long long)
private:
	std::stringstream stream;
};

class ELALogger
{
public:
	ELALogger()
	{
		this->productDir = "./";
		this->logLevel = LogLevel::info;
		this->logFilesCount = 0;
		this->ROTATE_LOG_SIZE = 10000000;
		this->kFormatSpecifierChar = '%';
		this->kFormatSpecifierCharValue = 'v';
		this->extension = "out";
		this->console = false;
		this->logFileName = LoggerUtil::GetModuleName();
	}

	void SetLogFileSize(long long logFileSize)
	{
		this->ROTATE_LOG_SIZE = logFileSize;
	}

	void SetFormatSpecifierChar(char format)
	{
		this->kFormatSpecifierChar = format;
	}

	void SetFormatSpecifierCharValue(char formatValue)
	{
		this->kFormatSpecifierCharValue = formatValue;
	}

	template <typename T, typename... Args>
	void log_(const char* s, const T& value, const Args&... args)
	{
		while (*s)
		{
			if (*s == kFormatSpecifierChar)
			{
				if (*(s + 1) == kFormatSpecifierChar)
				{
					++s;
				}
				else
				{
					if (*(s + 1) == kFormatSpecifierCharValue)
					{
						++s;
						msgBuilder << value;
						log_(++s, args...);
						return;
					}
				}
			}
			msgBuilder << *s++;
		}
	}

	template <typename T>
	void log_(const T& log)
	{
		msgBuilder << log;
	}

	template <typename T, typename... Args>
	void log(const char* s, const T& value, const Args&... args)
	{
		log_(s, value, args...);
	}

	template <typename T>
	void log(const T& log)
	{
		log_(log);
	}

	template <typename T, typename... Args>
	void info(const char* s, const T& value, const Args&... args)
	{
		logLock.lock();
		log(s, value, args...);
		WriteTemplateMessageToFile();
		logLock.unlock();
	}

	template <typename T, typename... Args>
	void error(const char* s, const T& value, const Args&... args)
	{
		logLock.lock();
		log(s, value, args...);
		WriteTemplateMessageToFile();
		logLock.unlock();
	}

	template <typename T, typename... Args>
	void trace(const char* s, const T& value, const Args&... args)
	{
		if (logLevel >= debug)
		{
			logLock.lock();
			log(s, value, args...);
			WriteTemplateMessageToFile();
			logLock.unlock();
		}
	}

	void error(char* message)
	{
		logLock.lock();
		WriteMessageToFile(message);
		logLock.unlock();
	}

	void info(char* message)
	{
		logLock.lock();
		WriteMessageToFile(message);
		logLock.unlock();
	}

	void trace(char* message)
	{
		if (logLevel >= debug)
		{
			logLock.lock();
			WriteMessageToFile(message);
			logLock.unlock();
		}
	}

	ELALogger* SetConsoleLogging()
	{
		console = true;
		return this;
	}

	void SetLogLevel(LogLevel level)
	{
		logLock.lock();
		logLevel = level;
		logLock.unlock();
	}

	void SetLogDir(std::string dirName)
	{
		productDir = dirName;
		char last = productDir.back();
		if (last != '/' && last != '\\')
		{
			productDir.append("/");
		}
	}

	void SetExtension(std::string ext)
	{
		extension = ext;
		char front = ext.front();
		if (front != '.')
		{
			extension.insert(0, 1, '.');
		}
	}

	std::string GetLogFile()
	{
		return this->fullLogFileName;
	}

	void SetLogFileName(std::string filename)
	{
		logLock.lock();
		std::stringstream actualFileName;
		actualFileName << filename << "." << extension;
		this->fullLogFileName = productDir + actualFileName.str();
		this->logFileName = filename;
		fptr = fopen(fullLogFileName.c_str(), "a+");
		if (fptr)
		{
			fclose(fptr);
		}
		logLock.unlock();
	}

	void SetFullPathForLogFile(std::string filename)
	{
		this->fullLogFileName = filename;
		this->logFileName = LoggerUtil::GetFileName(filename);
		this->extension = LoggerUtil::GetExtension(filename);
		this->productDir = LoggerUtil::GetParent(filename);
	}

	ELALogger* Start()
	{
		if (!console)
		{
			SetLogFileName(logFileName);
		}
		return this;
	}

private:
	void GetStream()
	{
		if (console)
		{
			fptr = stdout;
		}
		else
		{
			long long fileSize = LoggerUtil::GetFileDiskSize(fullLogFileName);
			if (fileSize >= ROTATE_LOG_SIZE)
			{
				RotateLogFile();
			}
			fptr = fopen(fullLogFileName.c_str(), "a+");
		}
	}

	void CloseStream()
	{
		fflush(fptr);
		if (!console)
		{
			fclose(fptr);
		}
	}

	void WriteMessageToFile(const char* message)
	{
		GetStream();
		if (fptr)
		{
			std::string logMessage = FormatLogMessage(message);
			fprintf(fptr, "%s\n", logMessage.c_str());
		}
		CloseStream();
	}

	void WriteTemplateMessageToFile()
	{
		std::string logMessage = msgBuilder.GetString().str();
		WriteMessageToFile(logMessage.c_str());
		msgBuilder.Clear();
	}

	std::string FormatLogMessage(const char* message)
	{
		time_t t;
		time(&t);
		struct tm* now = localtime(&t);

		std::stringstream ss;
		ss << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << "-" << now->tm_mday << " ";
		ss << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << " ";
		ss << "[" << LoggerUtil::GetThreadID() << "]";
		ss << message;
		return ss.str();
	}
    void RotateLogFile1()
    {
      ++logFilesCount;
      std::cout << logFilesCount << std::endl;
      if (logFilesCount >= 10)
      {
        logFilesCount = 1;
      }
      std::stringstream newFile;
      time_t t;
      time(&t);
      newFile << logFileName << "_" << logFilesCount << "." << extension;
      std::string fileName = productDir + newFile.str();
      remove(fileName.c_str());
      while (rename(fullLogFileName.c_str(), fileName.c_str()));
      std::stringstream actualFileName;
      time(&t);
      actualFileName << logFileName << "." << extension;
      fullLogFileName = productDir + actualFileName.str();
      fptr = fopen(fullLogFileName.c_str(), "a+");
      if (fptr)
      {
        fclose(fptr);
      }
    }
	void RotateLogFile()
	{
		++logFilesCount;
        std::cout << logFilesCount << std::endl;
		if (logFilesCount >= 10)
		{
			logFilesCount = 1;
		}
		std::stringstream newFile;
		time_t t;
		time(&t);
		newFile << logFileName << "_" << logFilesCount << "." << extension;
		std::string fileName = productDir + newFile.str();
		remove(fileName.c_str());
        while(rename(fullLogFileName.c_str(), fileName.c_str()));
		std::stringstream actualFileName;
		time(&t);
		actualFileName << logFileName << "." << extension;
		fullLogFileName = productDir + actualFileName.str();
		fptr = fopen(fullLogFileName.c_str(), "a+");
		if (fptr)
		{
			fclose(fptr);
		}
	}

	FILE* fptr;
	std::mutex logLock;
	LogLevel logLevel;
	std::string logFileName;
	std::string productDir;
	std::string fullLogFileName;
	std::string extension;
	bool console;
	int logFilesCount;

	long long ROTATE_LOG_SIZE;
	char kFormatSpecifierChar;
	char kFormatSpecifierCharValue;
	MessageBuilder msgBuilder;
};

#ifdef TEST


#endif
