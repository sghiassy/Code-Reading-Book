#!/bin/sh
# -----------------------------------------------------------------------------
# digest.bat - Digest password using the algorithm specificied
#
#   CATALINA_HOME (Optional) May point at your Catalina "build" directory.
#                 If not present, the current working directory is assumed.
#
#   JAVA_HOME     Must point at your Java Development Kit installation.
#
#   This script is assumed to run from the bin directory or have the
#   CATALINA_HOME env variable set.
#
# $Id: digest.sh,v 1.4 2001/09/12 00:54:28 craigmcc Exp $
# -----------------------------------------------------------------------------


# ----- Verify and Set Required Environment Variables -------------------------

if [ -z "$CATALINA_HOME" ] ; then
  ## resolve links - $0 may be a link to  home
  PRG=$0
  progname=`basename $0`
  
  while [ -h "$PRG" ] ; do
    ls=`ls -ld "$PRG"`
    link=`expr "$ls" : '.*-> \(.*\)$'`
    if expr "$link" : '.*/.*' > /dev/null; then
	PRG="$link"
    else
	PRG="`dirname $PRG`/$link"
    fi
  done
  
  CATALINA_HOME_1=`dirname "$PRG"`/..
  echo "Guessing CATALINA_HOME from digest.sh to ${CATALINA_HOME_1}" 
    if [ -d ${CATALINA_HOME_1}/conf ] ; then 
	CATALINA_HOME=${CATALINA_HOME_1}
	echo "Setting CATALINA_HOME to $CATALINA_HOME"
    fi
fi

if [ -z "$JAVA_HOME" ] ; then
  echo You must set JAVA_HOME to point at your Java Development Kit installation
  exit 1
fi


# ----- Cygwin Unix Paths Setup -----------------------------------------------

# Cygwin support.  $cygwin _must_ be set to either true or false.
case "`uname`" in
  CYGWIN*) cygwin=true ;;
  *) cygwin=false ;;
esac
 
# For Cygwin, ensure paths are in UNIX format before anything is touched
if $cygwin ; then
  [ -n "$CATALINA_HOME" ] &&
    CATALINA_HOME=`cygpath --unix "$CATALINA_HOME"`
    [ -n "$JAVA_HOME" ] &&
    JAVA_HOME=`cygpath --unix "$JAVA_HOME"`
fi


# ----- Set Up The System Classpath -------------------------------------------

CP="$CATALINA_HOME/server/lib/catalina.jar"

if [ -f "$JAVA_HOME/lib/tools.jar" ] ; then
  CP=$CP:"$JAVA_HOME/lib/tools.jar"
fi


# ----- Cygwin Windows Paths Setup --------------------------------------------

# convert the existing path to windows
if $cygwin ; then
   CP=`cygpath --path --windows "$CP"`
   CATALINA_HOME=`cygpath --path --windows "$CATALINA_HOME"`
   JAVA_HOME=`cygpath --path --windows "$JAVA_HOME"`
fi


# ----- Execute The Requested Command -----------------------------------------

echo "Using CLASSPATH:     $CP"
echo "Using CATALINA_HOME: $CATALINA_HOME"
echo "Using JAVA_HOME:     $JAVA_HOME"

if [ "$1" != "-a" ] || [ -z "$2" ] || [ -z "$3" ] ; then

  echo "Usage:  digest -a [algorithm] [credentials]"
  echo "Commands:"
  echo "  algorithm   -   The algorithm to use, i.e. MD5, SHA1"
  echo "  credentials -   The credentials to digest"

  exit 1

else

  shift

    $JAVA_HOME/bin/java -classpath $CP \
     org.apache.catalina.realm.JDBCRealm -a "$@"

fi
