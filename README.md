YaFSM
=====

YaFSM is a framework to handle Finite State Machines. The framework contains

* using SCXML definition to design Finite State Machines
* perl based generator to generate c++ code out of the XML description in pure c++ or using qt
* under development: c++ generator


YaFSM is a perl based script to generate Code for an Finite State Machine implementation out of an SCXML description of the FSM.

YaFSM features

* code generation for C++
*   full featured implementation supporting timer and events with Qt 4
*   possibility to implement timer and events with custom framework
*   examples and tests
*   works on linux and windows


Usage
-----

The YaFSM generator is a perl script bundle that is used to generate

* code

from a single scxml based FSM description.

To use it simple call the main perl script along with the command line arguments.
The complete argument list could be printed with the –man or –help command line option
YaFsm.pl –verbose


SCXML File
--------

Input for the YaFSM generator is a scxml description of the state engine. Default SCXML application is QtCreator Model Plugin.


Usage of the generator
----------------------

An example for use of the generator is given in src/perl/CmakeLists.txt

    perl -f YaFsm.pl --fsm=<xml-file> --gencode

This will generate the source code and viewer input files into subdirectories of the current directoy.

You can also give the name of the output directory via cmd line. To list all available options just call

    perl -f YaFsm.pl --help

Compilation
-----------

The fsm code generator has not to be build into an executable, cause it is a perl script. So its ready to use on linux and windows. On Windows you should use ActivePerl. On Linux you have to install the xmlsimple perl package from your distribution or also use ActivePerl.

Be aware that for unit tests the qt unittest framework is used and QT >= 4.5.2 is required.

The generated source code will not compile if used out of the example build tree, cause some include and source code files are missing. These files must be copied „by hand“ or by our build environment to the generated source code or e.g linked by a library. This has the advantage that you could use our own implementation and framework for the timer and event handling. Provided is an implementation for Qt4 (full featured)

