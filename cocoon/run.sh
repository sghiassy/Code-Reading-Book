#!/bin/sh
# -----------------------------------------------------------------------------
# run.sh - Unix Run Script for Apache Cocoon
#
# $Id: run.sh,v 1.2 2002/01/11 18:57:49 bloritsch Exp $
# -----------------------------------------------------------------------------

# ----- Verify and Set Required Environment Variables -------------------------

if [ "$JAVA_HOME" = "" ] ; then
  echo You must set JAVA_HOME to point at your Java Development Kit installation
  exit 1
fi

# ----- Verify and Set Required Environment Variables -------------------------

if [ "$TERM" = "cygwin" ] ; then
  S=';'
else
  S=':'
fi

if [ "$COCOON_LIB" = "" ] ; then
  COCOON_LIB=./lib
fi

if [ "$COCOON_WORK" = "" ] ; then
  COCOON_WORK=./work
  rm -rf $COCOON_WORK
  mkdir $COCOON_WORK
fi

# ----- Set Up The Runtime Classpath ------------------------------------------

CP=\"`echo $COCOON_LIB/*.jar | tr ' ' $S`$S$JAVA_HOME/lib/tools.jar$S$COCOON_WORK$S$CLASSPATH\"

# ----- Run Cocoon ------------------------------------------------------------

$JAVA_HOME/bin/java $COCOON_OPTS -classpath $CP org.apache.cocoon.Main $*



