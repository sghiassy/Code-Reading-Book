@echo off
rem ---------------------------------------------------------------------------
rem digest.bat - Digest password using the algorithm specificied
rem
rem   CATALINA_HOME (Optional) May point at your Catalina "build" directory.
rem                 If not present, the current working directory is assumed.
rem
rem   JAVA_HOME     Must point at your Java Development Kit installation.
rem
rem   This script is assumed to run from the bin directory or have the
rem   CATALINA_HOME env variable set.
rem
rem $Id: digest.bat,v 1.2 2001/09/10 20:12:58 ccain Exp $
rem ---------------------------------------------------------------------------


rem ----- Save Environment Variables That May Change --------------------------

set _CATALINA_HOME=%CATALINA_HOME%
set _CLASSPATH=%CLASSPATH%
set _CP=%CP%

rem ----- Verify and Set Required Environment Variables -----------------------

if not "%JAVA_HOME%" == "" goto gotJavaHome
echo You must set JAVA_HOME to point at your Java Development Kit installation
goto cleanup
:gotJavaHome

if not "%CATALINA_HOME%" == "" goto gotCatalinaHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\server\lib\catalina.jar" goto okCatalinaHome
set CATALINA_HOME=..
:gotCatalinaHome
if exist "%CATALINA_HOME%\server\lib\catalina.jar" goto okCatalinaHome
echo Cannot find catalina.jar in %CATALINA_HOME%\server\lib
echo Please check your CATALINA_HOME setting or run this script from the bin directory
goto cleanup
:okCatalinaHome


rem ----- Prepare Appropriate Java Execution Commands -------------------------

set _RUNJAVA="%JAVA_HOME%\bin\java"

rem ----- Set Up The Runtime Classpath ----------------------------------------

set CP=%CATALINA_HOME%\server\lib\catalina.jar
set CLASSPATH=%CP%
echo Using CLASSPATH: %CLASSPATH%


rem ----- Execute The Requested Command ---------------------------------------

if "%1" == "-a" (if "%2" neq "" (if "%3" neq "" goto doRun))

:doUsage
echo Usage:  digest -a [algorithm] [credentials]
echo Commands:
echo   algorithm   -   The algorithm to use, i.e. MD5, SHA1
echo   credentials -   The credential to digest
goto cleanup

:doRun
%_RUNJAVA% org.apache.catalina.realm.JDBCRealm %1 %2 %3
goto cleanup


rem ----- Restore Environment Variables ---------------------------------------

:cleanup
set CATALINA_HOME=%_CATALINA_HOME%
set _CATALINA_HOME=
set CLASSPATH=%_CLASSPATH%
set _CLASSPATH=
set CP=%_CP%
set _CP=
:finish
