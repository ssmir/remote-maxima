#!/bin/bash
. ./paths.sh

LD_LIBRARY_PATH=../lib DYLD_LIBRARY_PATH=../lib node --Ice.Config=$1.conf,maxima_path.conf &

echo $! > server.pid

