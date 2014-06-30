#!/bin/sh
# -----------------------------------------------------------------------------
# catalina.sh - Start/Stop Script for the CATALINA Server
#
# Environment Variable Prequisites
#
#   CATALINA_BASE (Optional) Base directory for resolving dynamic portions
#                 of a Catalina installation.  If not present, resolves to
#                 the same directory that CATALINA_HOME points to.
#
#   CATALINA_HOME (Optional) May point at your Catalina "build" directory.
#                 If not present, the current working directory is assumed.
#
#   CATALINA_OPTS (Optional) Java runtime options used when the "start",
#                 "stop", or "run" command is executed.
#
#   JAVA_HOME     Must point at your Java Development Kit installation.
#
#   JPDA_OPTS     (Optional) Java runtime options used when the "jpda start"
#                 command is executed.  Defaults to
#                 "-classic -Xdebug -Xnoagent -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n"
#
#   JSSE_HOME     (Optional) May point at your Java Secure Sockets Extension
#                 (JSSE) installation, whose JAR files will be added to the
#                 system class path used to start Tomcat.
#
# $Id: catalina.sh,v 1.20 2001/09/14 20:01:21 craigmcc Exp $
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
  echo "Guessing CATALINA_HOME from catalina.sh to ${CATALINA_HOME_1}" 
    if [ -d ${CATALINA_HOME_1}/conf ] ; then 
	CATALINA_HOME=${CATALINA_HOME_1}
	echo "Setting CATALINA_HOME to $CATALINA_HOME"
    fi
fi

if [ -z "$CATALINA_OPTS" ] ; then
  CATALINA_OPTS=""
fi

if [ -z "$JPDA_OPTS" ] ; then
  JPDA_OPTS="-classic -Xdebug -Xnoagent -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n"
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

CP="$CATALINA_HOME/bin/bootstrap.jar"

if [ -f "$JAVA_HOME/lib/tools.jar" ] ; then
  CP=$CP:"$JAVA_HOME/lib/tools.jar"
fi
if [ -f "$JSSE_HOME/lib/jsse.jar" ] ; then
  CP=$CP:"$JSSE_HOME/lib/jcert.jar":"$JSSE_HOME/lib/jnet.jar":"$JSSE_HOME/lib/jsse.jar"
fi


# ----- Cygwin Windows Paths Setup --------------------------------------------

# convert the existing path to windows
if $cygwin ; then
   CP=`cygpath --path --windows "$CP"`
   CATALINA_HOME=`cygpath --path --windows "$CATALINA_HOME"`
   JAVA_HOME=`cygpath --path --windows "$JAVA_HOME"`
fi


# ----- Set Up CATALINA_BASE If Necessary -------------------------------------

if [ -z "$CATALINA_BASE" ] ; then
  CATALINA_BASE=$CATALINA_HOME
fi


# ----- Execute The Requested Command -----------------------------------------

echo "Using CLASSPATH:     $CP"
echo "Using CATALINA_BASE: $CATALINA_BASE"
echo "Using CATALINA_HOME: $CATALINA_HOME"
echo "Using JAVA_HOME:     $JAVA_HOME"

if [ "$1" = "jpda" ] ; then
  CATALINA_OPTS="${CATALINA_OPTS} ${JPDA_OPTS}"
  shift
fi

if [ "$1" = "debug" ] ; then

  shift
  if [ "$1" = "-security" ] ; then
    shift
    $JAVA_HOME/bin/jdb \
       $CATALINA_OPTS \
       -sourcepath $CATALINA_HOME/../../jakarta-tomcat-4.0/catalina/src/share \
       -classpath $CP \
       -Dcatalina.base=$CATALINA_BASE \
       -Dcatalina.home=$CATALINA_HOME \
       org.apache.catalina.startup.Bootstrap "$@" start
  else
    $JAVA_HOME/bin/jdb \
       $CATALINA_OPTS \
       -sourcepath $CATALINA_HOME/../../jakarta-tomcat-4.0/catalina/src/share \
       -classpath $CP \
       -Dcatalina.base=$CATALINA_BASE \
       -Dcatalina.home=$CATALINA_HOME \
       org.apache.catalina.startup.Bootstrap "$@" start
  fi

elif [ "$1" = "embedded" ] ; then

  shift
  for i in ${CATALINA_HOME}/server/lib/*.jar ; do
    CP=$i:${CP}
  done
  for i in ${CATALINA_HOME}/common/lib/*.jar ; do
    CP=$i:${CP}
  done
  echo Embedded Classpath: $CP
  $JAVA_HOME/bin/java $CATALINA_OPTS -classpath $CP \
   -Dcatalina.base=$CATALINA_BASE \
   -Dcatalina.home=$CATALINA_HOME \
   org.apache.catalina.startup.Embedded "$@"

elif [ "$1" = "env" ] ; then

  export BP CATALINA_HOME CP
  exit 0

elif [ "$1" = "run" ] ; then

  shift
  if [ "$1" = "-security" ] ; then
    echo Using Security Manager
    shift
    $JAVA_HOME/bin/java $CATALINA_OPTS -classpath $CP \
     -Djava.security.manager \
     -Djava.security.policy==$CATALINA_BASE/conf/catalina.policy \
     -Dcatalina.base=$CATALINA_BASE \
     -Dcatalina.home=$CATALINA_HOME \
     org.apache.catalina.startup.Bootstrap "$@" start
  else
    $JAVA_HOME/bin/java $CATALINA_OPTS -classpath $CP \
     -Dcatalina.base=$CATALINA_BASE \
     -Dcatalina.home=$CATALINA_HOME \
     org.apache.catalina.startup.Bootstrap "$@" start
  fi

elif [ "$1" = "start" ] ; then

  shift
  touch $CATALINA_BASE/logs/catalina.out
  if [ "$1" = "-security" ] ; then
    echo Using Security Manager
    shift
    $JAVA_HOME/bin/java $CATALINA_OPTS -classpath $CP \
     -Djava.security.manager \
     -Djava.security.policy==$CATALINA_BASE/conf/catalina.policy \
     -Dcatalina.base=$CATALINA_BASE \
     -Dcatalina.home=$CATALINA_HOME \
     org.apache.catalina.startup.Bootstrap "$@" start \
     >> $CATALINA_BASE/logs/catalina.out 2>&1 &
  else
    $JAVA_HOME/bin/java $CATALINA_OPTS -classpath $CP \
     -Dcatalina.base=$CATALINA_BASE \
     -Dcatalina.home=$CATALINA_HOME \
     org.apache.catalina.startup.Bootstrap "$@" start \
     >> $CATALINA_BASE/logs/catalina.out 2>&1 &
  fi

elif [ "$1" = "stop" ] ; then

  shift
  $JAVA_HOME/bin/java $CATALINA_OPTS -classpath $CP \
   -Dcatalina.base=$CATALINA_BASE \
   -Dcatalina.home=$CATALINA_HOME \
   org.apache.catalina.startup.Bootstrap "$@" stop

else

  echo "Usage: catalina.sh ( env | run | start | stop)"
  echo "Commands:"
  echo "  debug             Start Catalina in a debugger"
  echo "  debug -security   Debug Catalina with a security manager"
  echo "  env               Set up environment variables that would be used"
  echo "  jpda start        Start Catalina under JPDA debugger"
  echo "  run               Start Catalina in the current window"
  echo "  run -security     Start in the current window with security manager"
  echo "  start             Start Catalina in a separate window"
  echo "  start -security   Start in a separate window with security manager"
  echo "  stop              Stop Catalina"
  exit 1

fi
