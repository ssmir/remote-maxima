import sys
import os, platform
import configure, variables, builders

cmdVars = variables.CreateDescription()
baseEnv = Environment(variables = cmdVars, tools = ['default', 'packaging', 'textfile'])
baseEnv.Tool('cxxtest', [baseEnv['CXXTEST_HOME'] + '/build_tools/SCons'],
    CXXTEST = baseEnv['CXXTEST_HOME'] + '/python/scripts/cxxtestgen',
    CXXTEST_CPPPATH = baseEnv['CXXTEST_HOME'],
    CXXTEST_OPTS = '--have-eh --abort-on-fail',
    CXXTEST_SUFFIX = 'Test.h')
builders.addSlice2cppBuilder(baseEnv)
builders.addMyInstallBuilder(baseEnv)

Help(cmdVars.GenerateHelpText(baseEnv))

baseEnv.Append(
    CPPPATH = ['#${VARDIR}/src', '#src', '$BOOST_INCLUDES', '$BOOST_PROCESS_INCLUDES'],
    LIBPATH = ['#lib'],
    LIBS = [], # to make clean working (undefined if configure is skipped)
    VARDIR = "build-${MY_PLATFORM}-${DEBUG and 'debug' or 'release'}",
    MY_PLATFORM = platform.system() + "-" + configure.runCommand('uname -m'),
    CCFLAGS = ['-Wall'],
    JNI_SUFFIX = baseEnv['SHLIBSUFFIX'])

if platform.system() == "Darwin":
    baseEnv['JNI_SUFFIX'] = '.jnilib'

if not baseEnv.GetOption('clean'):
    baseEnv = configure.Configure(baseEnv)
    
debugEnv = baseEnv.Clone(DEBUG = 1)
debugEnv.Append(CCFLAGS = ['-g', '-O0'])

releaseEnv = baseEnv.Clone(DEBUG = 0)
releaseEnv.Append(CCFLAGS = ['-O3'])

debugEnv['DO_PACKAGE'] = True

for env in [debugEnv, releaseEnv]:
    env.VariantDir('${VARDIR}', '.')
    env.SConscript(dirs = ['${VARDIR}/test', '${VARDIR}/examples', '${VARDIR}/java',
        '${VARDIR}/slice', '${VARDIR}/src'], exports = 'env')
    if 'DO_PACKAGE' in env:
        env.Package(
            NAME = 'remote-maxima-${MY_PLATFORM}',
            VERSION = '0.1',
            SUMMARY = 'Simple C++ CAS Maxima API and a RPC service based on it.',
            DESCRIPTION    = """""",
            VENDOR = 'Sergey Smirnov <sasmir@gmail.com>',
            X_MSI_LANGUAGE = 'en-us')

