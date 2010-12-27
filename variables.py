import os, platform
from SCons.Script import *

def CreateDescription():
    cmdVars = Variables('custom.py')
    if platform.system() == "Windows":
        pass#cmdVars.AddVariables(BoolVariable("NO_WCHAR", "Build with wchar_t=short", 1))
    else:
        cmdVars.AddVariables(
            ('CXXTEST_HOME', 'Path to a directory which contains cxxtest'),
            ('BOOST_PROCESS_INCLUDES', 'Path to a directory which contains the boost includes directory'),
            ('BOOST_INCLUDES', 'Path to a directory which contains the boost includes directory'),
            ('ICE_INCLUDES', 'C++ headers for ZeroC Ice'),
            ('ICE_LIB_PATH', 'ZeroC Ice libraries path'),
            ('SLICE2CPP', 'ZeroC Ice slice2cpp translator'),
            ('SLICE_INCLUDES', 'ZeroC Ice built-in slices'))
    return cmdVars

