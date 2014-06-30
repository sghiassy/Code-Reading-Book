#!/bin/sh

echo
echo "Apache Cocoon Build System"
echo "--------------------------"

cp ./lib/core/xalan*.jar ./tools/lib
cp ./lib/core/xerces*.jar ./tools/lib
cp ./lib/core/xml-api*.jar ./tools/lib
cp ./lib/optional/jtidy*.jar ./tools/lib

chmod u+x ./tools/bin/antRun
chmod u+x ./tools/bin/ant

unset ANT_HOME

CP=$CLASSPATH
export CP
unset CLASSPATH

$PWD/tools/bin/ant -logger org.apache.tools.ant.NoBannerLogger -emacs $@ 

CLASSPATH=$CP
export CLASSPATH
