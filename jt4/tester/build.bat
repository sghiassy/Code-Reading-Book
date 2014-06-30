@echo off
rem ---------------------------------------------------------------------------
rem build.bat - Build Script for Tester
rem
rem Environment Variable Prerequisites:
rem
rem   ANT_HOME         Must point at your Ant installation [../jakarta-ant]
rem
rem   ANT_OPTS         Command line options to the Java runtime
rem                    that executes Ant [NONE]
rem
rem   JAVA_HOME        Must point at your Java Development Kit [REQUIRED]
rem
rem   XERCES_HOME      Must point at your Xerces installation
rem                    [../../xerces-1_3_0]
rem
rem $Id: build.bat,v 1.3 2001/03/13 19:06:06 craigmcc Exp $
rem ---------------------------------------------------------------------------


rem ----- Save Environment Variables ------------------------------------------

set _CLASSPATH=%CLASSPATH%
set _ANT_HOME=%ANT_HOME%
set _XERCES_HOME=%XERCES_HOME%


rem ----- Verify and Set Required Environment Variables -----------------------


if not "%JAVA_HOME%" == "" goto gotJavaHome
echo You must set JAVA_HOME to point at your Java Development Kit installation
goto cleanup
:gotJavaHome

if not "%ANT_HOME%" == "" goto gotAntHome
set ANT_HOME=../../jakarta-ant
:gotAntHome

if not "%XERCES_HOME%" == "" goto gotXercesHome
set XERCES_HOME=../../xerces-1_3_0
:gotXercesHome


rem ----- Set Up The Runtime Classpath ----------------------------------------

if not "%CLASSPATH%" == "" set CLASSPATH=%CLASSPATH%;
set CLASSPATH=%CLASSPATH%;%ANT_HOME%\lib\ant.jar;%JAVA_HOME%\lib\tools.jar


rem ----- Execute The Requested Build -----------------------------------------

%JAVA_HOME%\bin\java %ANT_OPTS% org.apache.tools.ant.Main -Dant.home="%ANT_HOME%" -Djava.home="%JAVA_HOME%" -Dxerces.home="%XERCES_HOME%" %1 %2 %3 %4 %5 %6 %7 %8 %9


rem ----- Restore Environment Variables ---------------------------------------

:cleanup
set CLASSPATH=%_CLASSPATH%
set _CLASSPATH=
set ANT_HOME=%_ANT_HOME%
set _ANT_HOME=
set XERCES_HOME=%_XERCES_HOME%
set _XERCES_HOME=
:finish

