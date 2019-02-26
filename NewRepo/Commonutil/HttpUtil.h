#pragma once
#ifdef WINDOWS
#include <ELAWindows.h>
#include <ELALogger.h>

#include <winhttp.h>
#include <string>

class Win32HttpUtil
{
private:
	HINTERNET hsession, hconnect, hrequest;
	std::string UpdateResponseData();
	std::string responseData;
public:
	static std::wstring useragent;
	static std::wstring server;
	static INTERNET_PORT port;
	static bool secure;
	static ELALogger* logger;
	Win32HttpUtil();
	~Win32HttpUtil();
	DWORD OpenConnection();
	DWORD InitGetRequest(const wchar_t* object);
	DWORD SendUpdate(const wchar_t* object, bool WinHttpReceiveResponse = false);
	std::string GetResponseString();
	void CleanRequest();
	void CloseConnection();
	static void SetParams(std::wstring useragent, std::wstring server, ELALogger* logger, INTERNET_PORT port = 0,
	                      bool secure = true);
};
#endif
