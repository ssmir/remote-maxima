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

#ifndef __SOEXPORTDEFS_H__
#define __SOEXPORTDEFS_H__

// Shared library support
#ifdef WIN32
  #define NODEIMPORT __declspec(dllimport)
  #define NODEEXPORT __declspec(dllexport)
  #define NODEDLLLOCAL
  #define NODEDLLPUBLIC
#else
  #ifdef GCC_HASCLASSVISIBILITY
    #define NODEIMPORT __attribute__ ((visibility("default")))
    #define NODEEXPORT __attribute__ ((visibility("default")))
    #define NODEDLLLOCAL __attribute__ ((visibility("hidden")))
    #define NODEDLLPUBLIC __attribute__ ((visibility("default")))
  #else
    #define NODEIMPORT
    #define NODEEXPORT
    #define NODEDLLLOCAL
    #define NODEDLLPUBLIC
  #endif
#endif

// Define NODEAPI for DLL builds
#ifdef NODEDLL
  #ifdef NODEDLL_EXPORTS
    #define NODEAPI NODEEXPORT
  #else
    #define NODEAPI NODEIMPORT
  #endif // NODEDLL_EXPORTS
#else
  #define NODEAPI
#endif // NODEDLL

#endif // __SOEXPORTDEFS_H__

