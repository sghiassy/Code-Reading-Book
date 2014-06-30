@echo off
rem ----------------------------------------------------------------------------
rem build.bat - Win32 Build Script for Apache Cocoon
rem
rem $Id: build.bat,v 1.15 2002/01/25 20:58:20 vgritsenko Exp $
rem ----------------------------------------------------------------------------

rem ----- Copy Xalan and Xerces for the build system    ------------------------
copy lib\core\xerces*.jar tools\lib
copy lib\core\xalan*.jar tools\lib
copy lib\core\xml-api*.jar tools\lib
copy lib\optional\jtidy*.jar tools\lib

rem ----- Verify and Set Required Environment Variables ------------------------

rem if not "%ANT_HOME%" == "" goto gotAntHome
set OLD_ANT_HOME=%ANT_HOME%
set ANT_HOME=tools
:gotAntHome

call %ANT_HOME%\bin\ant %1 %2 %3 %4 %5 %6 %7 %8 %9

set ANT_HOME=%OLD_ANT_HOME%
set OLD_ANT_HOME=
