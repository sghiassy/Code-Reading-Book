#!/bin/sh
# -----------------------------------------------------------------------------
# jasper.sh - Global Script Jasper
#
# Environment Variable Prequisites
#
# Environment Variable Prequisites:
#   JASPER_HOME (Optional)
#       May point at your Jasper "build" directory.
#       If not present, the current working directory is assumed.
#   JASPER_OPTS (Optional) 
#       Java runtime options
#   JAVA_HOME     
#       Must point at your Java Development Kit installation.
#
# $Id: jasper.sh,v 1.5 2001/09/13 15:22:45 craigmcc Exp $
# -----------------------------------------------------------------------------


# ----- Verify and Set Required Environment Variables -------------------------

if [ -z "$JASPER_HOME" ] ; then
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
  
  JASPER_HOME_1=`dirname "$PRG"`/..
  echo "Guessing JASPER_HOME from catalina.sh to ${JASPER_HOME_1}" 
    if [ -d ${JASPER_HOME_1}/conf ] ; then 
	JASPER_HOME=${JASPER_HOME_1}
	echo "Setting JASPER_HOME to $JASPER_HOME"
    fi
fi

if [ "$JASPER_OPTS" = "" ] ; then
  JASPER_OPTS=""
fi

if [ "$JAVA_HOME" = "" ] ; then
  echo "You must set JAVA_HOME to point at your Java Development Kit installation"
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
  [ -n "$JASPER_HOME" ] &&
    JASPER_HOME=`cygpath --unix "$JASPER_HOME"`
    [ -n "$JAVA_HOME" ] &&
    JAVA_HOME=`cygpath --unix "$JAVA_HOME"`
fi


# ----- Set Up The System Classpath -------------------------------------------

CP="$JASPER_HOME/classes"
for i in $JASPER_HOME/jasper/*.jar ; do
  CP=$CP:"$i"
done
for i in $JASPER_HOME/lib/*.jar ; do
  CP=$CP:"$i"
done
CP=$CP:"$JASPER_HOME/common/classes"
for i in $JASPER_HOME/common/lib/*.jar ; do
  CP=$CP:"$i"
done


# ----- Cygwin Windows Paths Setup --------------------------------------------

# convert the existing path to windows
if $cygwin ; then
   CP=`cygpath --path --windows "$CP"`
   JASPER_HOME=`cygpath --path --windows "$JASPER_HOME"`
   JAVA_HOME=`cygpath --path --windows "$JAVA_HOME"`
fi


# ----- Execute The Requested Command -----------------------------------------

echo "Using CLASSPATH:   $CP"
echo "Using JASPER_HOME: $JASPER_HOME"
echo "Using JAVA_HOME:   $JAVA_HOME"

if [ "$1" = "jspc" ] ; then

  shift
  java $JASPER_OPTS -classpath $CP \
   -Djasper.home=$JASPER_HOME \
   org.apache.jasper.JspC "$@"

else

  echo "Usage: jasper.sh [ jspc ]"
  echo "Commands:"
  echo   jspc - Run the jasper offline JSP compiler
  exit 1

fi
