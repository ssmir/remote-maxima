﻿import os, subprocess, urllib
from SCons.Script import *
import ConfigureJNI

def runCommand(cmd):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    out = p.stdout.read().strip()
    return out
    
def CheckIce(context, includes_path, libs_path):
    source = """
    #include <Ice/Ice.h>
    int main() {
        Ice::PropertyDict dict;
        return 0;
    }"""
    context.env.Append(CPPPATH = [includes_path], LIBPATH = [libs_path],
        LIBS = ['IceUtil', 'Ice'])

    result = context.TryLink(source, '.cc')
    if not result:
        context.env['LIBS'].pop()
        context.env.Append(LIBS = ['ZeroCIce'])
        result = context.TryLink(source, '.cc')
    
    if not result or (includes_path == '' and libs_path == ''):
        context.env['CPPPATH'].pop()
        context.env['LIBPATH'].pop()
    if not result:
        context.env['LIBS'].pop()
        context.env['LIBS'].pop()
    return result
    
def FindAndCheckIce(context):
    context.Message('Checking for ZeroC Ice... ')
    result = CheckIce(context, '', '')
    if result:
        context.Result(result)
        return result
        
    if 'ICE_INCLUDES' in context.env:
        result = CheckIce(context, context.env['ICE_INCLUDES'], context.env['ICE_LIB_PATH'])
        context.Result(result)
        return result
    
    for base in ['/usr', '/usr/local', '/opt/local']:
        result = CheckIce(context, base + '/include', base + '/lib')
        if result:
            context.Result(result)
            return result
    context.Result(0)
    return 0
    
def FindHeader(context, header, cmdArg):
    context.Message('Checking for %s... ' % header)
    if cmdArg in context.env:
        cpp_path = context.env[cmdArg]
    else:
        for base in ['/usr/include', '/usr/local', '/opt/local/include']:
            path = runCommand("find %s -wholename '*/%s' -print -quit" % (base, header))
            if path:
                break
        if not path:
            context.Result(0)
            return 0
        cpp_path = path[:-1-len(header)]
    context.env.Append(CPPPATH = [cpp_path])
    result = context.TryCompile('#include <%s/%s>' % (cpp_path, header), '.cc')
    if not result:
        context.env['CPPPATH'].remove(cpp_path)
    context.Result(result)
    return result

def FindJNIHeaders(context):
    context.Message('Finding jni.h path... ')
    result = ConfigureJNI.ConfigureJNI(context.env)
    context.Result(result)
    return result
    
def downloadProgressHook(blocks, blockSize, fileSize):
    sys.stdout.write("Downloaded %.2f MB of %d    \r" %
        (blocks * blockSize / 1024. / 1024., fileSize))

def fetchURL(target, url):
    if not os.path.exists(target):
        print 'Fetching %s into %s' % (url, target)
        urllib.urlretrieve(url, target, downloadProgressHook)

def Configure(env):
    conf = env.Configure(custom_tests = {'FindHeader' : FindHeader,
        'FindAndCheckIce' : FindAndCheckIce, 'FindJNIHeaders' : FindJNIHeaders})
    if conf.env['ICE_ENABLE'] and conf.FindAndCheckIce():
        conf.env.Append(CPPDEFINES = ['HAS_ICE'], LIBS = ['IceUtil', 'IceSSL'])
    if not conf.FindJNIHeaders():
        print 'jni.h is essential'
        Exit(1)
    if env['EVEREST_HOME']:
        try:
            os.mkdir('src/everest/lib')
        except:
            pass
        fetchURL('src/everest/lib/jython-2.5.2rc3.jar',
            'http://binaries.remote-maxima.googlecode.com/hg/jython-2.5.2rc3.jar')
    return conf.Finish()

