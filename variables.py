import os, platform
from SCons.Script import *

def CreateDescription():
    cmdVars = Variables('custom.py')
    if platform.system() == "Windows":
        pass#cmdVars.AddVariables(BoolVariable("NO_WCHAR", "Build with wchar_t=short", 1))
    else:
        cmdVars.AddVariables(
PathVariable('BOOST_PROCESS_INCLUDES', 'Path to a directory which contains the boost includes directory',
    '', PathVariable.PathIsDir),
PathVariable('BOOST_INCLUDES', 'Path to a directory which contains the boost includes directory',
    '/usr/include', PathVariable.PathIsDir),
PathVariable('BOOST_LIB_PATH', 'Directory containing Boost libraries',
    '/usr/lib', PathVariable.PathIsDir),
PathVariable('ICE_INCLUDES', 'C++ headers for ZeroC Ice',
    '/usr/include', PathVariable.PathIsDir),
PathVariable('ICE_LIB_PATH', 'ZeroC Ice libraries path',
    '/usr/lib', PathVariable.PathIsDir),
PathVariable('SLICE2CPP', 'ZeroC Ice slice2cpp translator',
    WhereIs('slice2cpp', os.environ['PATH'])),
PathVariable('SLICE_INCLUDES', 'ZeroC Ice built-in slices',
    '/usr/include', PathVariable.PathIsDir),
BoolVariable('ICE_ENABLE', 'Enable/disable building of Ice dependent parts', 'no'),
PathVariable('MAXIMA_APP', 'Path to the Maxima executable for examples and tests',
    WhereIs('maxima', os.environ['PATH']), PathVariable.PathIsFile),
PackageVariable('CXXTEST_HOME', 'CxxTest distribution root', 'no')
)
    return cmdVars

