package YaFsmCodeGen;

use strict;
use warnings;
#use Cwd;
#use Scalar::Util;
use File::Path;
#use File::Find;
use File::Basename;
use YaFsmCodeGenCppOO;
use YaFsmScxmlCodeGenCppOO;
#use File::Copy;
#use FindBin;
#use XML::Parser;
#use XML::Simple;
#use threads;
#use Time::HiRes qw(gettimeofday tv_interval);
#use ProBuildMake;

#use vars qw($VERSION @ISA @EXPORT);
#require Exporter;
#@ISA = qw(Exporter);
#@EXPORT = qw(createProdDirs $isWin $SrcProdDirDebug);
#$VERSION = 1.0;


BEGIN {
    use Exporter   ();
    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    # set the version for version checking
    $VERSION     = 3.0;

    @ISA         = qw(Exporter);
    @EXPORT      = qw();#&createProdDirs
    %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],

    # your exported package globals go here,
    # as well as any optionally exported functions
#    @EXPORT_OK   = qw($RootPath);
}


our $MAJOR_VERSION = 3;
our $MINOR_VERSION = 0;
our @EXPORT_OK;



sub writeCodeFiles
{
  my $FSMName = shift;
  my $generateScxml = shift;
  if( $generateScxml)
  {
    YaFsmScxmlCodeGenCppOO::writeCodeFiles($FSMName);
  }
  else
  {
    YaFsmCodeGenCppOO::writeCodeFiles($FSMName);
  }
}




1;
