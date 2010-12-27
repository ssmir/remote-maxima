﻿import sys
import os, platform
import configure, variables
import SCons.Scanner.IDL

cmdVars = variables.CreateDescription()
baseEnv = Environment(variables = cmdVars, tools=['default', 'packaging'])
Help(cmdVars.GenerateHelpText(baseEnv))

def idl2many_emitter(target, source, env):
    newTarget = []
    src = source[0]
    dst = target[0]
    if str(src).endswith('.slice'):
        for suffix in ['.cc', '.h']:
            newTarget.append(str(dst) + suffix)
    return (newTarget, source)

baseEnv['BUILDERS']['Slice2cpp'] = Builder(
    action="${SLICE2CPP} --source-ext=cc -I${SLICE_INCLUDES} -I. ${SLICEFLAGS} --output-dir ${TARGET.dir} ${SOURCE}",
    src_suffix = '.slice',
    emitter = idl2many_emitter,
    source_scanner = SCons.Scanner.IDL.IDLScan(),
    single_source = 1
)

baseEnv.Append(
    CPPPATH = ['.', '#include', '$BOOST_INCLUDES', '$BOOST_PROCESS_INCLUDES'],
    LIBPATH = ['#lib'],
    VARDIR = "build-${MY_PLATFORM}-${DEBUG and 'debug' or 'release'}",
    SLICEFLAGS = '--include-dir IARnet')

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
    env.SConscript(dirs = ['${VARDIR}/slice', '${VARDIR}/src'], exports = 'env')

baseEnv.Package(
    NAME = 'remote-maxima-${MY_PLATFORM}',
    VERSION = '0.1',
    SUMMARY = 'Simple C++ CAS Maxima API and a RPC service based on it.',
    DESCRIPTION    = """""",
    VENDOR = 'Sergey Smirnov <sasmir@gmail.com>',
    X_MSI_LANGUAGE = 'en-us')

#debugEnv.Alias('debug', map(lambda x: x['buildtarget'], msvsPacks[debugEnv['DEBUG']]))
#releaseEnv.Alias('release', map(lambda x: x['buildtarget'], msvsPacks[releaseEnv['DEBUG']]))

