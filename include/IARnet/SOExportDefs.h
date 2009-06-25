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

