$Id: README.txt,v 1.17 2001/08/25 03:51:27 craigmcc Exp $

                   The Tomcat 4.0 Servlet/JSP Container
                   ====================================

This subproject contains a server that conforms to the Servlet 2.3 and
JSP 1.2 specifications from Java Software.  It includes the following contents:

  BUILDING.txt                Instructions for building from sources
  LICENSE                     Apache Software License for this release
  README.txt                  This document
  RELEASE-NOTES-*.txt         Release Notes for this (and previous) releases
                              of Tomcat 4.0
  RUNNING.txt                 Instructions for installing Tomcat, as well as
                              starting and stopping the server
  bin/                        Binary executables and scripts
  classes/                    Unpacked classes global to web applications
  common/                     Classes available to both Catalina internal
                              classes and web applications:
    classes/                  Unpacked common classes
    lib/                      Common classes in JAR files
  conf/                       Configuration files
  jasper/                     JAR files visible only in the Jasper classloader
  lib/                        Classes in JAR files global to web applications
  logs/                       Destination directory for log files
  server/                     Internal Catalina classes and their dependencies
    classes/                  Unpacked classes (internal only)
    lib/                      Classes packed in JAR files (internal only)
  webapps/                    Base directory containing web applications
                              included with Tomcat 4.0
  work/                       Scratch directory used by Tomcat for holding
                              temporary files and directories

If you wish to build the Tomcat server from a source distribution,
please consult the documentation in "BUILDING.txt".

If you wish to install and run a binary distribution of the Tomcat server,
please consult the documentation in "RUNNING.txt".


                      Acquiring Tomcat 4.0 Releases
                      =============================

Nightly Builds
--------------

Nightly Builds of Tomcat 4.0 are built from the most recent CVS sources each
evening (Pacific Time).  The filename of the downloadable file includes the
date it was created (in YYYYMMDD format).  These builds are available at:

Binary:  http://jakarta.apache.org/builds/jakarta-tomcat-4.0/nightly/
Source:  http://jakarta.apache.org/builds/jakarta-tomcat-4.0/nightly/src/


Release Builds
--------------

Release Builds of Tomcat 4.0 are created and released periodically, and
announced to the interested mailing lists.  Each release build resides in its
own directories.  For example, the Tomcat 4.0-beta-7 release is available at:

Binary:  http://jakarta.apache.org/builds/jakarta-tomcat-4.0/release/v4.0-b7/
Source:  http://jakarta.apache.org/builds/jakarta-tomcat-4.0/release/v4.0-b7/src/

