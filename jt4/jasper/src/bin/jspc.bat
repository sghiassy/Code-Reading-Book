@echo off
rem ---
rem jspc.bat - Script ro run the Jasper "offline JSP compiler"
rem
rem $Id: jspc.bat,v 1.1 2000/08/12 00:52:05 pierred Exp $
rem ---

rem --- Save Env Variables
set _JASPER_HOME=%JASPER_HOME%

rem --- Set Env Variables
if not "%JASPER_HOME%" == "" goto gotJasperHome
echo JASPER_HOME not set. Assuming parent directory.
set JASPER_HOME=..
:gotJasperHome

rem --- got jasper.bat?
if exist %JASPER_HOME%\bin\jasper.bat goto gotJasperBat
echo Using JASPER_HOME: %JASPER_HOME%
echo jasper.bat does not exist. Please specify JASPER_HOME properly.
goto restore
:gotJasperbat

rem --- jspc is invoked via jasper's generic script
%JASPER_HOME%\bin\jasper jspc %1 %2 %3 %4 %5 %6 %7 %8 %9

rem --- Restore Env Variables
:restore
set JASPER_HOME=%_JASPER_HOME%
set _JASPER_HOME=

:eof
