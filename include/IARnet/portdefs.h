#ifndef __PORTDEFS_H__
#define __PORTDEFS_H__

#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <netdb.h>
#endif

#endif // __PORTDEFS_H__
