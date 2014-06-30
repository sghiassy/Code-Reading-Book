#!/bin/sh
# -----------------------------------------------------------------------------
# tester.sh - Execute Test Application Client
#
# Environment Variable Prequisites
#
#   CATALINA_HOME (Optional) May point at your Catalina "build" directory.
#                 If not present, the current working directory is assumed.
#
#   CATALINA_OPTS (Optional) Java runtime options used when the "start",
#                 "stop", or "run" command is executed.
#
#   JAVA_HOME     Must point at your Java Development Kit installation.
#
# $Id: tester.sh,v 1.3 2001/02/21 21:39:50 craigmcc Exp $
# -----------------------------------------------------------------------------


# ----- Verify and Set Required Environment Variables -------------------------

if [ "$CATALINA_HOME" = "" ] ; then
  CATALINA_HOME=`pwd`
fi

if [ "$CATALINA_OPTS" = "" ] ; then
  CATALINA_OPTS=""
fi

if [ "$ANT_HOME" = "" ] ; then
  ANT_HOME=../jakarta-ant
fi

if [ "$JAVA_HOME" = "" ] ; then
  echo You must set JAVA_HOME to point at your Java Development Kit installation
  exit 1
fi

# ----- Set Up The System Classpath -------------------------------------------

CP=$CATALINA_HOME/webapps/tester/WEB-INF/lib/tester.jar:$CATALINA_HOME/server/lib/jaxp.jar:$CATALINA_HOME/server/lib/crimson.jar:$ANT_HOME/lib/ant.jar

echo Using CLASSPATH: $CP


# ----- Execute The Requested Command -----------------------------------------

$JAVA_HOME/bin/java $CATALINA_OPTS -classpath $CP org.apache.tools.ant.Main \
 -Dant.home=$ANT_HOME \
 -Dcatalina.home=$CATALINA_HOME \
 -buildfile $CATALINA_HOME/bin/tester.xml \
 "$@"
