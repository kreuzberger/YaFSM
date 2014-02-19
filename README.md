YaFSM
=====

YaFSM is a framework to handle Finite State Machines. The framework contains

* XML Definition to design Finite State Machines
* perl base generator to generate c++ code out of the XML description in pure c++ or using qt
* qt based viewer for generated diagrams of the fsm



YaFSM is a perl based script to generate Code and a graphical representation for an Finite State Machine implementation out of an XML description of the FSM.

YaFSM features

* code generation for C++
*   generation of fsm graphical representation via html and SVG
*   standalone viewer for the generated fsm graphical representation
*   full featured implementation supporting timer and events with Qt 4
*   possibility to implement timer and events with custom framework
*   examples and tests
*   works on linux and windows




Usage
-----

The YaFSM generator is a perl script bundle that is used to generate

* code
* descriptions
* images

from a single xml based FSM description.

To use it simple call the main perl script along with the command line arguments.
The complete argument list could be printed with the –man or –help command line option
YaFsm.pl –verbose

XML File
--------

Input for the YaFSM generator is a xml description of the state engine. Details on the xml tags are given in Apppendix xx. You can edit the file with any text editor you like.
To verify the xml file you can test it agains the given xsd schema file ,that can be found in the xml directory.

On Linux you could use xmllint, on windows e.g. xmlstarlet

xmllint -s yafsm.xsd <yourfsm.xml>

On Linux you could use xmllint, on windows e.g. xmlstarlet

Usage of the generator
----------------------

An example for use of the generator is given in src/perl/CmakeLists.txt

    perl -f YaFsm.pl --fsm=<xml-file> --genview --gencode

This will generate the source code and viewer input files into subdirectories of the current directoy.

You can also give the name of the output directory via cmd line. To list all available options just call

    perl -f YaFsm.pl --help

Compilation
-----------

The fsm code generator has not to be build into an executable, cause it is a perl script. So its ready to use on linux and windows. On Windows you should use ActivePerl. On Linux you have to install the xmlsimple perl package from your distribution or also use ActivePerl.

The YaFsmViewer can be compiled using the provided cmake scripts. Be aware that for unit tests the unittest++ framework is used and for the YaFsmViewer QT >= 4.5.2 is required.

The generated source code will not compile if used out of the example build tree, cause some include and source code files are missing. These files must be copied „by hand“ or by our build environment to the generated source code or e.g linked by a library. This has the advantage that you could use our own implementation and framework for the timer and event handling. Provided is an implementation for Qt4 (full featured) and a standard C++ implementation (no timers and events could be used in the fsm)


The YaFsmViewer
---------------

The  files for the YaFsmViewer are generated into a seperated directory. This directory contains several files for each state hierarchy level

* <state>.dot: Generated dot file for the state, use to generate html maps and image files with Graphviz dot
* <state>.html: Html file with a map definition to display in the viewers QWebKit View
* <state>.<GeneratedImageType>: Generated image file used by the map, default are SVG image files have full zooming capability in the viewer

To view the files in the viewer, just open the generated file „index.txt“ in the viewer or start the viewer with the file path from commandline.

![](https://raw2.github.com/kreuzberger/YaFSM/master/doc/yafsmviewer/YaFsmViewer_TrafficLightStateRun.png)
