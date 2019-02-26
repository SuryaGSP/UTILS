#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#define VC_EXTRALEAN

#ifndef WIN32_WINNT
#define WIN32_WINNT 0x0501
#endif

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#ifdef LOGAGENT
#define _UNICODE
#define UNICODE
#endif

#include <winsock2.h>
#include <Windows.h>
#include <winioctl.h>
