import sys
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
    if str(src).endswith('.ice'):
        for suffix in ['.cc', '.h']:
            newTarget.append(str(dst) + suffix)
    return (newTarget, source)

baseEnv['BUILDERS']['Slice2cpp'] = Builder(
    action="${SLICE2CPP} --source-ext=cc -I${SLICE_INCLUDES} -I. ${SLICEFLAGS} --output-dir ${TARGET.dir} ${SOURCE}",
    src_suffix = '.ice',
    emitter = idl2many_emitter,
    source_scanner = SCons.Scanner.IDL.IDLScan(),
    single_source = 1
)

def MyInstallBld(env, target, source):
    if 'DO_PACKAGE' in env:
        return env.Install(target, source)
    return []
baseEnv['BUILDERS']['MyInstall'] = MyInstallBld

baseEnv.Append(
    CPPPATH = ['.', '#include', '$BOOST_INCLUDES', '$BOOST_PROCESS_INCLUDES'],
    LIBPATH = ['#lib'],
    VARDIR = "build-${MY_PLATFORM}-${DEBUG and 'debug' or 'release'}")

baseEnv.Append(
    MY_PLATFORM = platform.system() + "-" + configure.runCommand('uname -m'),
    CCFLAGS = ['-Wall'],
    JNI_SUFFIX = baseEnv['SHLIBSUFFIX'])

if platform.system() == "Darwin":
    baseEnv['JNI_SUFFIX'] = '.jnilib'

if not baseEnv.GetOption('clean'):
    baseEnv = configure.Configure(baseEnv)
    
debugEnv = baseEnv.Clone(DEBUG = 1)
debugEnv.Append(CCFLAGS = ['-g', '-O0'])
debugEnv['PROGSUFFIX'] = 'd' + debugEnv['PROGSUFFIX']
debugEnv['SHLIBSUFFIX'] = 'd' + debugEnv['SHLIBSUFFIX']
debugEnv['JNI_SUFFIX'] = 'd' + debugEnv['JNI_SUFFIX']

releaseEnv = baseEnv.Clone(DEBUG = 0)
releaseEnv.Append(CCFLAGS = ['-O3'])

debugEnv['DO_PACKAGE'] = True

for env in [debugEnv, releaseEnv]:
    env.VariantDir('${VARDIR}', '.', duplicate = 0)
    env.SConscript(dirs = ['${VARDIR}/java', '${VARDIR}/slice', '${VARDIR}/src'], exports = 'env')
    if 'DO_PACKAGE' in env:
        env.Package(
            NAME = 'remote-maxima-${MY_PLATFORM}',
            VERSION = '0.1',
            SUMMARY = 'Simple C++ CAS Maxima API and a RPC service based on it.',
            DESCRIPTION    = """""",
            VENDOR = 'Sergey Smirnov <sasmir@gmail.com>',
            X_MSI_LANGUAGE = 'en-us')

#debugEnv.Alias('debug', map(lambda x: x['buildtarget'], msvsPacks[debugEnv['DEBUG']]))
#releaseEnv.Alias('release', map(lambda x: x['buildtarget'], msvsPacks[releaseEnv['DEBUG']]))

