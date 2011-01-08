#!/bin/bash
CLASSPATH="../everest/dist/lib/everest.jar:../JythonService/JythonService.jar:../remote-maxima/lib/MaximaInstance.jar:$CLASSPATH" \
$HOME/jython2.5.1/jython -c "from MaximaService import startTest; startTest()"

