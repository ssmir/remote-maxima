import sys
import os, platform
import configure, variables

cmdVars = variables.CreateDescription()
baseEnv = Environment(variables = cmdVars, tools=['default', 'packaging'])
Help(cmdVars.GenerateHelpText(baseEnv))

baseEnv.Append(
    CPPPATH = ['.', '#include', '$BOOST_INCLUDES', '$BOOST_PROCESS_INCLUDES'],
    LIBPATH = ['#lib'],
    VARDIR = "build-${MY_PLATFORM}-${DEBUG and 'debug' or 'release'}")

if platform.system() == "Darwin":
    baseEnv.Append(
        MY_PLATFORM = "darwin-" + configure.runCommand('uname -m'),
        CCFLAGS = ['-Wall'],
        JNI_SUFFIX = '.jnilib')
    
    if not baseEnv.GetOption('clean'):
        baseEnv = configure.Configure(baseEnv)
    
    debugEnv = baseEnv.Clone(DEBUG = 1)
    debugEnv.Append(CCFLAGS = ['-g', '-O0'])
    debugEnv['PROGSUFFIX'] = 'd' + debugEnv['PROGSUFFIX']
    debugEnv['SHLIBSUFFIX'] = 'd' + debugEnv['SHLIBSUFFIX']
    debugEnv['JNI_SUFFIX'] = 'd' + debugEnv['JNI_SUFFIX']
    
    
    releaseEnv = baseEnv.Clone(DEBUG = 0)
    releaseEnv.Append(CCFLAGS = ['-O3'])
elif platform.system() == "Linux":
    baseEnv.Append(
        MY_PLATFORM = "linux-" + configure.runCommand('uname -m'),
        LINKFLAGS = '-Wl,-rpath,$ICE_HOME/lib:$BOOST_LIB:.',
        CCFLAGS = ['-Wall'],
        CPPPATH = ['$JAVA_HOME/include/linux'],
        LIBS = ['Ice', 'IceUtil', 'IceSSL'])
    
    if not baseEnv.GetOption('clean'):
        baseEnv = configure.Configure(baseEnv)
    
    debugEnv = baseEnv.Clone(DEBUG = 1)
    debugEnv.Append(CCFLAGS = ['-g', '-O0'])
    debugEnv['PROGSUFFIX'] = 'd' + debugEnv['PROGSUFFIX']
    
    releaseEnv = baseEnv.Clone(DEBUG = 0)
    releaseEnv.Append(CCFLAGS = ['-O3'])

for env in [debugEnv, releaseEnv]:
    env.VariantDir('${VARDIR}', '.', duplicate = 0)
    env.SConscript(dirs = ['${VARDIR}/src'], exports = 'env')

baseEnv.Package(
    NAME = 'remote-maxima-${MY_PLATFORM}',
    VERSION = '0.1',
    SUMMARY = 'Simple C++ CAS Maxima API and a RPC service based on it.',
    DESCRIPTION    = """""",
    VENDOR = 'Sergey Smirnov <sasmir@gmail.com>',
    X_MSI_LANGUAGE = 'en-us')

#debugEnv.Alias('debug', map(lambda x: x['buildtarget'], msvsPacks[debugEnv['DEBUG']]))
#releaseEnv.Alias('release', map(lambda x: x['buildtarget'], msvsPacks[releaseEnv['DEBUG']]))

