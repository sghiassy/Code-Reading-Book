@echo off
rem build.bat -- Build Script for the "Hello, World" Application
rem $Id: build.bat,v 1.2 2001/08/14 18:50:09 pier Exp $

set _CP=%CP%

rem Identify the custom class path components we need
set CP=%CATALINA_HOME%\lib\ant.jar;%CATALINA_HOME%\lib\servlet.jar
set CP=%CATALINA_HOME%\lib\jaxp.jar;%CATALINA_HOME%\lib\parser.jar
set CP=%CP%;%JAVA_HOME%\lib\tools.jar

rem Execute ANT to perform the requird build target
java -classpath %CP%;%CLASSPATH% org.apache.tools.ant.Main -Dtomcat.home=%CATALINA_HOME% %1 %2 %3 %4 %5 %6 %7 %8 %9

set CP=%_CP%
set _CP=
