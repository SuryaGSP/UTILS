#ifdef WINDOWS
#include <HttpUtil.h>
#include <iostream>

#pragma comment (lib, "Winhttp.lib")

bool Win32HttpUtil::secure;
INTERNET_PORT Win32HttpUtil::port;
std::wstring Win32HttpUtil::useragent;
std::wstring Win32HttpUtil::server;
ELALogger* Win32HttpUtil::logger;

Win32HttpUtil::Win32HttpUtil() : hsession(nullptr), hconnect(nullptr), hrequest(nullptr)
{
	responseData = "";
	OpenConnection();
}

Win32HttpUtil::~Win32HttpUtil()
{
	CloseConnection();
}

DWORD Win32HttpUtil::OpenConnection()
{
	hsession = WinHttpOpen(useragent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, nullptr, nullptr, NULL);
	if (!hsession)
	{
		logger->error("Http Open failed : %v ", GetLastError());
		return S_FALSE;
	}
	hconnect = WinHttpConnect(hsession, server.c_str(), port, 0);
	if (!hconnect)
	{
		logger->error("Http connect failed : %v ", GetLastError());
		if (hsession) WinHttpCloseHandle(hsession);
		return S_FALSE;
	}
	return S_OK;
}

void Win32HttpUtil::CloseConnection()
{
	if (hconnect) WinHttpCloseHandle(hconnect);
	if (hsession) WinHttpCloseHandle(hsession);
}

DWORD Win32HttpUtil::InitGetRequest(const wchar_t* object)
{
	hrequest = WinHttpOpenRequest(hconnect, L"GET", object, nullptr, nullptr, nullptr,
	                              secure ? WINHTTP_FLAG_SECURE : 0);
	if (!hrequest)
	{
		logger->error("Http WinHttpOpenRequest failed : %v ", GetLastError());
		CloseConnection();
		return S_FALSE;
	}
	DWORD dwOptions =
		SECURITY_FLAG_IGNORE_CERT_CN_INVALID
		| SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
		| SECURITY_FLAG_IGNORE_UNKNOWN_CA
		| SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
	WinHttpSetOption(hrequest, WINHTTP_OPTION_SECURITY_FLAGS,
	                 &dwOptions, sizeof(DWORD));
	DWORD dwTOOption = 0x00000001;
	WinHttpSetOption(hrequest, WINHTTP_OPTION_CONNECT_TIMEOUT,
	                 &dwTOOption, sizeof(DWORD));
	return S_OK;
}

void Win32HttpUtil::CleanRequest()
{
	if (hrequest) WinHttpCloseHandle(hrequest);
}

void Win32HttpUtil::SetParams(std::wstring useragent, std::wstring server, ELALogger* logger, unsigned short int port,
                              bool secure)
{
	Win32HttpUtil::logger = logger;
	Win32HttpUtil::useragent = useragent;
	Win32HttpUtil::server = server;

		Win32HttpUtil::port = port;
		Win32HttpUtil::secure = secure;
	logger->info("Useragent : %v, server : %v, port : %v, secure : %v", useragent, Win32HttpUtil::server,
	             Win32HttpUtil::port, Win32HttpUtil::secure);
}

DWORD Win32HttpUtil::SendUpdate(const wchar_t* object, bool receiveReponse)
{
	if (hconnect)
	{
		DWORD result = S_FALSE;
		result = InitGetRequest(object);
		if (result == S_FALSE)
		{
			logger->error("SendUpdate: InitGetRequest failed (%v) for %v", GetLastError(), object);
			return S_FALSE;
		}
		result = WinHttpSendRequest(hrequest, nullptr, 0, nullptr, NULL, NULL, NULL);
		if (result)
		{
			if (WinHttpReceiveResponse(hrequest, nullptr))
			{
				DWORD dwStatusCode = 0;
				DWORD dwSize = sizeof(dwStatusCode);

				WinHttpQueryHeaders(hrequest,
				                    WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
				                    nullptr,
				                    &dwStatusCode, &dwSize, nullptr);

				if (dwStatusCode != 200)
				{
					logger->error("SendUpdate: WinHttpQueryHeaders returns error response code : %v for %v",
					              dwStatusCode, object);
				}
				else
				{
					result = S_OK;
					if (receiveReponse)
					{
						responseData = UpdateResponseData();
					}
				}
			}
			else
			{
				logger->error("SendUpdate: WinHttpReceiveResponse failed (%v) for %v", GetLastError(), object);
			}
			CleanRequest();
		}
		else
		{
			logger->error("SendUpdate: WinHttpSendRequest failed (%v) for %v", GetLastError(), object);
		}
		return result;
	}
	logger->error("SendUpdate: Cannot send data. HCONNECT not initialized");
	return S_FALSE;
}

std::string Win32HttpUtil::UpdateResponseData()
{
	DWORD size = 0;
	DWORD downloaded = 0;
	LPSTR outBuffer;
	if (!WinHttpQueryDataAvailable(hrequest, &size))
	{
		logger->error("Error in WinHttpQueryDataAvailable, while getting response size: %v", GetLastError());
		return "";
	}

	if (!size)
	{
		logger->info("No available response data");
		return "";
	}

	outBuffer = new char[size + 1];
	if (!outBuffer)
	{
		logger->error("Out of memory error in allocating space for out buffer, buffer size: %v", size);
		return "";
	}

	ZeroMemory(outBuffer, size + 1);
	if (!WinHttpReadData(hrequest, (LPVOID)outBuffer, size, &downloaded))
	{
		logger->error("Error in WinHttpReadData: %v", GetLastError());
	}

	std::string response = std::string(outBuffer);
	logger->trace("UpdateResponseData: The Respone received : %v", response);
	delete[]outBuffer;
	return response;
}

std::string Win32HttpUtil::GetResponseString()
{
	return responseData;
}

#endif
