@echo off
rem ---------------------------------------------------------------------------
rem tester.bat - Execute Test Application Client
rem
rem Environment Variable Prequisites:
rem
rem   ANT_HOME       Ant runtime directory
rem
rem   CATALINA_HOME (Optional) May point at your Catalina "build" directory.
rem                 If not present, the current working directory is assumed.
rem
rem   CATALINA_OPTS (Optional) Java runtime options used when the "start",
rem                 "stop", or "run" command is executed.
rem
rem   JAVA_HOME     Must point at your Java Development Kit installation.
rem
rem $Id: tester.bat,v 1.4 2001/02/21 21:39:50 craigmcc Exp $
rem ---------------------------------------------------------------------------


rem ----- Save Environment Variables That May Change --------------------------

set _ANT_HOME=%ANT_HOME%
set _CATALINA_HOME=%CATALINA_HOME%
set _CLASSPATH=%CLASSPATH%
set _CP=%CP%


rem ----- Verify and Set Required Environment Variables -----------------------

if not "%JAVA_HOME%" == "" goto gotJavaHome
echo You must set JAVA_HOME to point at your Java Development Kit installation
goto cleanup
:gotJavaHome

if not "%ANT_HOME%" == "" goto gotAntHome
set ANT_HOME=../../jakarta-ant
:gotAntHome

if not "%CATALINA_HOME%" == "" goto gotCatalinaHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okCatalinaHome
set CATALINA_HOME=..
:gotCatalinaHome
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okCatalinaHome
echo Unable to determine the value of CATALINA_HOME
goto cleanup
:okCatalinaHome


rem ----- Prepare Appropriate Java Execution Commands -------------------------

if not "%OS%" == "Windows_NT" goto noTitle
set _RUNJAVA="%JAVA_HOME%\bin\java"
goto gotTitle
:noTitle
set _RUNJAVA="%JAVA_HOME%\bin\java"
:gotTitle

rem ----- Set Up The Runtime Classpath ----------------------------------------

set CP=%CATALINA_HOME%\webapps\tester\WEB-INF\lib\tester.jar;%CATALINA_HOME%\server\lib\jaxp.jar;%CATALINA_HOME%\server\lib\crimson.jar;%ANT_HOME%\lib\ant.jar
set CLASSPATH=%CP%
echo Using CLASSPATH: %CLASSPATH%


rem ----- Execute The Requested Command ---------------------------------------

%_RUNJAVA% %CATALINA_OPTS% org.apache.tools.ant.Main -Dant.home="%ANT_HOME%" -Dcatalina.home="%CATALINA_HOME%" -buildfile "%CATALINA_HOME%\bin\tester.xml" %1 %2 %3 %4 %5 %6 %7 %8 %9

:cleanup
set CATALINA_HOME=%_CATALINA_HOME%
set _CATALINA_HOME=
set CLASSPATH=%_CLASSPATH%
set _CLASSPATH=
set CP=%_CP%
set ANT_HOME=%_ANT_HOME%
set _ANT_HOME=
set _RUNJAVA=
:finish
