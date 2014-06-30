@echo off
rem ---------------------------------------------------------------------------
rem shutdown.bat - Stop Script for the CATALINA Server
rem
rem $Id: shutdown.bat,v 1.4 2000/10/16 19:13:14 craigmcc Exp $
rem ---------------------------------------------------------------------------

set _CATALINA_HOME=%CATALINA_HOME%
if not "%CATALINA_HOME%" == "" goto gotCatalinaHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\bin\catalina.bat" goto gotCatalinaHome
set CATALINA_HOME=..
if exist "%CATALINA_HOME%\bin\catalina.bat" goto gotCatalinaHome
echo Unable to determine the value of CATALINA_HOME
goto cleanup
:gotCatalinaHome
"%CATALINA_HOME%\bin\catalina" stop %1 %2 %3 %4 %5 %6 %7 %8 %9
:cleanup
set CATALINA_HOME=%_CATALINA_HOME%
set _CATALINA_HOME=
