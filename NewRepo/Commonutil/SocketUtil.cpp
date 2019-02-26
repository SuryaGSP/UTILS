#include <SocketUtil.h>
#include <ELALogger.h>
#include <CommonUtil.h>
#ifdef UNIX
#include <sys/types.h>
#include <event2/util.h>
#include <openssl/err.h>
#include <ifaddrs.h>
#endif
#include <ELALogger.h>
#include <iostream>
#include <algorithm>

#include <stdio.h>
#include <string.h>

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 46
#endif

namespace SocketUtil
{
	//Wraning : Assumes WSAStartup() is called allready
	bool GetHostIPFromHostName(char* ip, const char* host)
	{
		hostent* record = gethostbyname(host);
		if (record)
		{
			in_addr* address = reinterpret_cast<in_addr *>(record->h_addr);
			strcpy(ip, inet_ntoa(*address));
			return true;
		}
		else
		{
			return false;
		}
	}

	bool GetHostNameFromHostIP(char* host, const char* ip)
	{
		unsigned long ipNum = inet_addr(ip);
		hostent* record = gethostbyaddr(reinterpret_cast<const char *>(&ipNum), 4, AF_INET);
		if (record)
		{
			strcpy(host, record->h_name);
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string SockaddrToStringIP(const sockaddr_storage* paddr, ELALogger *logger)
	{
		char pszaddr[INET_ADDRSTRLEN];
		DWORD dwSize = INET_ADDRSTRLEN;
#ifdef WINDOWS
		DWORD rs = WSAAddressToStringA((sockaddr*)paddr, sizeof(sockaddr_storage), nullptr, pszaddr, &dwSize);
		if (rs)
		{
			HandleSocketError(std::string(__FUNCTION__) + "::WSAAddressToStringA", logger);
		}
		else
		{
			char* colon = strchr(pszaddr, ':');
			int colonindex = colon - pszaddr;
			pszaddr[colonindex] = '\0';
		}
#else
		inet_ntop(AF_INET, &(((struct sockaddr_in*)paddr)->sin_addr), pszaddr, INET_ADDRSTRLEN);
#endif
		return std::string(pszaddr);
	}

	PORT SockaddrToUIntPort(const sockaddr_storage* paddr)
	{
		sockaddr_in* pAddrIn = (struct sockaddr_in*)paddr;
		return ntohs(pAddrIn->sin_port);
	}

	void LogError(std::string func, std::string code, std::string msg, ELALogger *logger)
	{
		logger->error("[%v][Error]%v: %v", func, code, msg);
	}


	void PrintHexDump(int length, char* buffer, ELALogger *logger)
	{
		logger->info("Received %v bytes", length);
		int i, count, index;
		char rgbDigits[] = "0123456789abcdef";
		char rgbLine[100];

		for (index = 0; length; length -= count, buffer += count, index += count)
		{
			char cbLine;
			count = (length > 16) ? 16 : length;

			sprintf(rgbLine, "%4.4x  ", index);
			cbLine = 6;

			for (i = 0; i < count; i++)
			{
				rgbLine[cbLine++] = rgbDigits[buffer[i] >> 4];
				rgbLine[cbLine++] = rgbDigits[buffer[i] & 0x0f];
				if (i == 7)
				{
					rgbLine[cbLine++] = ':';
				}
				else
				{
					rgbLine[cbLine++] = ' ';
				}
			}
			for (; i < 16; i++)
			{
				rgbLine[cbLine++] = ' ';
				rgbLine[cbLine++] = ' ';
				rgbLine[cbLine++] = ' ';
			}

			rgbLine[cbLine++] = ' ';

			for (i = 0; i < count; i++)
			{
				if (buffer[i] < 32 || buffer[i] > 126 || buffer[i] == '%')
				{
					rgbLine[cbLine++] = '.';
				}
				else
				{
					rgbLine[cbLine++] = buffer[i];
				}
			}

			rgbLine[cbLine++] = 0;
			logger->info("%v", rgbLine);
		}
	}

#ifdef WINDOWS
	void HandleOtherError(std::string func_name, DWORD hResult, ELALogger *logger)
	{
		LPSTR message = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		               nullptr, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&message),
		               0, nullptr);
		if (message)
		{
			LogError(func_name, CommonUtil::WmiErrorValToHexString(hResult), message, logger);
			LocalFree(message);
		}
		else
		{
			LogError(func_name, CommonUtil::WmiErrorValToHexString(hResult), "Unknown Error", logger);
		}
	}

	DWORD HandleSocketError(std::string func_name, ELALogger *logger)
	{
		int result = WSAGetLastError();
		HandleOtherError(func_name, result, logger);
		return result;
	}

	DWORD HandleError(std::string func_name, ELALogger *logger)
	{
		int result = GetLastError();
		HandleOtherError(func_name, result, logger);
		return result;
	}
#else
	void HandleOtherError(std::string func_name, DWORD hResult, ELALogger *logger)
	{
		LogError(func_name, std::to_string(hResult), strerror(hResult), logger);
	}

	DWORD HandleError(std::string func_name, ELALogger *logger)
	{
		int result = errno;
		HandleOtherError(func_name, result, logger);
		return result;
	}

	DWORD HandleLibeventError(std::string func_name, ELALogger *logger)
	{
		int result = EVUTIL_SOCKET_ERROR();
		LogError(func_name, std::to_string(result), evutil_socket_error_to_string(result), logger);
		return result;
	}

	DWORD HandleOpenSSLError(std::string func_name, ELALogger *logger)
	{
		DWORD result = ERR_peek_last_error();
		LogError(func_name, std::to_string(result), ERR_error_string(result, NULL), logger);
		return result;
	}

	DWORD HandleSocketError(std::string func_name, ELALogger *logger)
	{
		return HandleError(func_name, logger);
	}

	void GetInterfaceAddresses(std::vector<std::string> &ips)
	{
		struct ifaddrs *ifap, *ifa;
		struct sockaddr_in *sa;
		getifaddrs(&ifap);
		for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr->sa_family == AF_INET) {
				sa = (struct sockaddr_in *) ifa->ifa_addr;
				char *addr = inet_ntoa(sa->sin_addr);
				if (std::find(ips.begin(), ips.end(), addr) == ips.end())
				{
					ips.push_back(addr);
				}
			}
		}

		freeifaddrs(ifap);

	}
#endif
}
