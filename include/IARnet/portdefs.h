/*  remote-maxima - a CAS Maxima C++ API and a Grid-service
	Copyright (C) 2007-2009 Sergey Smirnov <sasmir (at) gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published
    by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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
