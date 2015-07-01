remote-maxima is portable and works at least on Mac OS X, Linux, Windows XP. However projects/scripts to build remote-maxima on Windows haven't been added to the repository yet. We use [SCons](http://www.scons.org/) build system which is written in Python. Currently there are following dependencies:
  * [SCons](http://www.scons.org/) >= 2.0.1
  * [Python](http://www.python.org/) >= 2.6 (not 3.x)
  * [Maxima](http://maxima.sourceforge.net/) >= 5.22.1 built with SBCL (preferred), GCL or CLISP
  * [Boost](http://www.boost.org/) >= 1.36.0
  * [Boost.Process](http://svn.boost.org/svn/boost/sandbox/process) (bundled with the source distribution)
  * (optional) [CxxTest](http://cxxtest.tigris.org/) (trunk from subversion repo) for some unit tests (bundled with the source distribution)
  * (optional) [EveREST](https://www.assembla.com/spaces/mathcloud/documents) RESTful web-service engine for HTTP-based service (bundled with the source distribution)
  * (optional) [ZeroC Ice](http://www.zeroc.com/ice.html) >= 3.2.0 for RPC service

## Building on Linux/Mac OS X ##
Prerequisites:
  * Install SCons using your package managements system or manually from its [site](http://www.scons.org/download.php)
  * Install [Boost](http://www.boost.org/) with compiled system, regex and filesystem libraries. It's better and easier to install Boost using a package managements system rather than building it from source. **(Actually, on Debian or Ubuntu one should install** _boost-build_ **AND some** _libboost-dev_ **packages)**

Building remote-maxima:
  1. Download and extract a [source tarball](http://code.google.com/p/remote-maxima/downloads/list) (it already contains cxxtest, boost-process and everest).
  1. Modify build variables in custom.py if Boost or other paths are not standard.
  1. Execute:
```
$ cd remote-maxima
$ scons
```
Build should end without errors.

### Installing EveREST service ###
Install the service in the EveREST instance specified by the EVEREST\_HOME build variable (set in custom.py):
```
$ scons install-everest-service
```
If the action was not completely successful and the following message was returned:
```
Please update Everest config manually adding service description from maxima-service.json and updating maximaPath inside
```
Then insert the contents of `src/everest/maxima-service.json` file to the services section of the `${EVEREST_HOME}/everest/everest.conf`. Especially note the comma before the block copied from maxima-service.json. Also modify maximaPath from `/opt/local/bin/maxima` to the path to the Maxima executable on your system:
```

{
name: fib,
description: "Fibonacci numbers calculator. An advanced example of service implemented in Java.",
inputs: {
n: {type: integer, title: Number}
},
outputs: {
fib: {type: integer, title: Fib(N)}
},

implementation: {
type: java,
class: everest.test.FibService,
config: {maxN: 100}
}
},
{
"name" : "maxima",
"description" : "CAS Maxima REST-service accepting one Maxima expresion at a time",
"inputs" : {
"command" : {"type" : "string", "title" : "Input expression"},
"script.mac" : {"type" : "file", "optional" : true,
"title" : "Script to load before executing the command"},
"data.lsp" : {"type" : "file", "optional" : true,
"title" : "Lisp script to load after the script file but before the command"}
},
"outputs" : {
"result" : {"type" : "string", "title" : "Result returned by the command"},
"output.lsp" : {"type" : "file",
"title" : "Output file (output.lsp) which was used by the command"}
},
"implementation" : {
"type" : "java",
"class" : "ru.isa.dcs.ssmir.JythonService",
"config" : {
"pythonPackage" : "MaximaService",
"pythonClass" : "MaximaService",
*"maximaPath" : "/opt/local/bin/maxima"*

}
}
}

]

}
```

Then it will be possible to start the EveREST container:
```

$ cd _$EVEREST_HOME_

$ ./start.sh```
And [try](http://localhost:9999) the service in a web browser.

### Building with Ice service ###
  1. Install [ZeroC Ice](http://www.zeroc.com/download.html). It can be usually installed using a package manager.
  1. Edit custom.py to make the Ice paths correct:
```
# Build Ice part
ICE_ENABLE = "y"

# Ice paths
ICE_INCLUDES = "/usr/include"
ICE_LIB_PATH = "/usr/lib"
SLICE_INCLUDES = "/usr/share/Ice-3.3.1/slice"
SLICE2CPP = "/usr/bin/slice2cpp"
```
  1. Build with `scons` command as before.
  1. The service can be tested the following way:
```
$ cd remote-maxima-Linux-i686-0.1/share/remote-maxima/examples/
$ ./localtest.sh
```

## Building on Windows XP ##

TBD