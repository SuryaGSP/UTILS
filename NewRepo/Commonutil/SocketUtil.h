#pragma once
#ifdef WINDOWS
#include <ELAWindows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <string>
#include <vector>
#include <ELALogger.h>

#ifdef WINDOWS
#pragma comment(lib, "ws2_32.lib")
#endif

enum class PROTOCOL : char
{
	NONE = -1,
	UDP = 0,
	TCP = 1,
	TLS = 2
};

enum class POLLTYPE
{
	NIO,
	BIO
};

#define INADDR(a) ((struct sockaddr_in*)&a)->sin_addr
#define INPORT(p) ntohs(p.sin_port);

#ifdef WINDOWS
#define ELA_SOCKET SOCKET
typedef int socklen_t;
#else
#define ELA_SOCKET int
#define DWORD unsigned long
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

typedef unsigned short int PORT;

typedef struct
{
	PORT port;
	std::string ip;
} PortPair;

enum ELA_STATUS
{
	ELA_FAILURE = 0,
	ELA_SUCCESS = 1
};

typedef struct
{
	ELA_SOCKET hdle;
	PROTOCOL protocol;

	std::string localAddress; /* Local appln. address */
	std::string remoteAddress; /* Remote appln. address */

	PORT localPortNum; /* Local appln. port number */
	PORT remotePortNum;

	sockaddr_storage localSockAddressIn;
	sockaddr_storage remoteSockAddressIn;

	bool connected;
	void* association; // could be server or connection depending on the context
	// To be changed if confusing.
} IPInfo;

namespace SocketUtil
{
	bool GetHostIPFromHostName(char* ip, const char* host);
	bool GetHostNameFromHostIP(char* host, const char* ip);
	std::string SockaddrToStringIP(const sockaddr_storage* paddr, ELALogger *logger);
	PORT SockaddrToUIntPort(const sockaddr_storage* paddr);
	DWORD HandleError(std::string func_name, ELALogger *logger);
	DWORD HandleSocketError(std::string func_name, ELALogger *logger);
	void HandleOtherError(std::string func_name, DWORD hResult, ELALogger *logger);
	void PrintHexDump(int length, char* buffer, ELALogger *logger);
#ifdef WINDOWS
	struct InitWSAStartup
	{
		signed short retVal;
		InitWSAStartup(ELALogger *logger)
		{
			WORD wvers;
			WSADATA wdata;
			wvers = MAKEWORD(1, 1);
			retVal = WSAStartup(wvers, &wdata);

			if (retVal != 0)
			{
				HandleSocketError(std::string(__FUNCTION__) + "::WSAStartup", logger);
			}
		}

		~InitWSAStartup()
		{
			WSACleanup();
		}
	};

#else UNIX
	DWORD HandleLibeventError(std::string func_name,ELALogger *logger);
	void GetInterfaceAddresses(std::vector<std::string>&);
#endif
}
