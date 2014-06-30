@echo HSQLDB build file for jdk 1.1.x and above
@echo for all jdk's include the path to jdk1.x.x\bin in your system path statement
@echo for jdk1.1.x also set the system classpath to include the path to
@echo    jdk1.1.x\lib\classes.zip on your system
@echo example: classpath=c:\jkd1.1.8\lib\classes.zip

javac -O -classpath %classpath%;.;..\lib *.java org\hsqldb\*.java org\hsqldb\util\*.java
jar -cf ..\lib\hsqldb.jar *.class .\org\hsqldb\*.class .\org\hsqldb\util\*.class
del *.class
del org\hsqldb\*.class
del org\hsqldb\util\*.class
pause
