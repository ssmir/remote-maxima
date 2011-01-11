#!/bin/bash
FILENAME=remote-maxima-src-0.1

mkdir -p extern
svn export http://svn.boost.org/svn/boost/sandbox/process extern/boost-process
svn export http://cxxtest.tigris.org/svn/cxxtest/trunk extern/cxxtest --username guest
if [[ ! -e extern/everest.zip ]]; then
    wget -O extern/everest.zip http://www.assembla.com/spaces/mathcloud/documents/cLOWOKM7yr35T2eJe5cbLr/download/cLOWOKM7yr35T2eJe5cbLr
fi
cd extern
7za -y x everest.zip
cd ..

JYTHON=jython-2.5.2rc3.jar
JYTHON_LIB=src/everest/lib/$JYTHON

if [[ ! -e $JYTHON_LIB ]]; then
    wget -O $JYTHON_LIB http://binaries.remote-maxima.googlecode.com/hg/$JYTHON
fi

mv custom.py custom.py.tmp
cp custom.py.skel custom.py

TRACKED=$(hg status -c -m -a -d | sed 's/^[A-Z] \(.*\)$/\1/')

FILELIST="extern/boost-process extern/cxxtest extern/everest $TRACKED $JYTHON_LIB custom.py"

OUT=remote-maxima

for x in $FILELIST; do
    FILELIST2="$FILELIST2 $OUT/$x"
done

rm -f $FILENAME.7z $FILENAME.t*

cd ..

7za a $OUT/$FILENAME.tar $FILELIST2
7za a $OUT/$FILENAME.tbz $OUT/$FILENAME.tar
7za a $OUT/$FILENAME.tgz $OUT/$FILENAME.tar

cd $OUT
mv custom.py.tmp custom.py

