package YaFsmParser;

use strict;
use warnings;
use Cwd;
#use Scalar::Util;
use File::Path;
#use File::Find;
use File::Basename;
use File::Copy;
#use FindBin;
#use XML::Parser;
#use XML::Simple;
#use threads;
#use Time::HiRes qw(gettimeofday tv_interval);
#use ProBuildMake;
use YaFsmViewerGen;
use YaFsmCodeGen;
use FindBin;

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

our $gFSMFileName;
our $gFSMGenView=0;
our $gFSMGenDotType='svg';
our $gFSMGenCode=0;
our $gFSMViewOutPath;
our $gFSMCodeOutPath;
our %gFSMActions;
our %gFSMTriggers;
our @gFSMStates;
our %gFSMTimers;
our @gFSMEvents;
our $gFSM;
our $gFSMName;
our %gFSMMembers;
our @gFSMIncludes;
our $gImageExt="png";

#my @gDotImages=("enter.png","exit.png","timerstart.png","timerstop.png");
my @gDotImages=("enter.$gImageExt","exit.$gImageExt","timerstart.$gImageExt","timerstop.$gImageExt");

my $gStateLevel=-1;

our $isWin = 0;
if( $^O eq "MSWin32" )
{
  $isWin = 1;
}
our $slash = "/";
$slash = "\\" if $isWin;

sub init
{
  my $fsmname = shift;
  my $fsmbasename=basename($fsmname,'.xml');
  if((!defined $gFSMViewOutPath) && (!defined $gFSMCodeOutPath))
  {
    $gFSMViewOutPath= cwd() . '/' .lc($fsmbasename).'/view';
    $gFSMCodeOutPath= cwd() . '/' .lc($fsmbasename).'/code';
  }

  if($gFSMGenView && (defined $gFSMViewOutPath))
  {
    mkpath( $gFSMViewOutPath, {verbose => 1, mode => 0755}) if (!(-d $gFSMViewOutPath));
    my $imageOutPath = $gFSMViewOutPath . '/images';

    mkpath( $imageOutPath, {verbose => 1, mode => 0755}) if (!(-d $imageOutPath));

    my $imageInPath = $FindBin::Bin .'/images';
    foreach my $img (@gDotImages)
    {
      if(!copy("$imageInPath/$img", "$imageOutPath/$img"))
      {
        YaFsm::printFatal("$imageInPath/$img -> $imageOutPath/$img: $!");
      }
    }
  }

  if($gFSMGenCode && (defined $gFSMCodeOutPath))
  {
    mkpath( $gFSMCodeOutPath, {verbose => 1, mode => 0755}) if (!(-d $gFSMCodeOutPath));
  }
}


sub genCfg()
{
#  my @newBuildCfgArray = ({ name => "",
#                            uninstall => 0,
#                            filter => 0,
#                          });

#  return @newBuildCfgArray;
}

sub readFSM
{
  my $filename = shift;
  undef $gFSMFileName;
  if( -f $filename)
  {
    eval "use XML::Simple";
    if($@)
    {
      YaFsm::printWarn $@;
      YaFsm::printFatal("Missing required package XML::Simple");
    }

    if($YaFsm::gVerbose)
    {
      eval "use Data::Dumper";
      if($@)
      {
        YaFsm::printWarn $@;
        YaFsm::printFatal("Missing required package Data::Dumper");
      }
    }


    # create object
    my $xml = new XML::Simple (KeyAttr=>[]);

    # read XML file
    # force array is useful for configurations that have only one entry and are not parsed into
    # array by default. So we ensure that the buildcfg always is an array!
  #  my $xmlContent = eval{$xml->XMLin("$filename", SuppressEmpty => '',ForceArray => qr/buildcfg$/)};
    my $xmlContent = eval{$xml->XMLin("$filename", ForceArray => [qw(state transition action trigger timer event member include)])};
    #my $xmlContent = eval{$xml->XMLin("$filename", ForceArray => qr/state$/ )};
    if ($@)
    {
      # parse error messages look like this:
      # not well-formed at line 10, column 8, byte 382 at d:/bin/perl/site/lib/XML/Parser.pm line 183
      $@ =~ /(.*) at .*? line \d+\.?\s*$/;
      my $errmsg = $@;
      YaFsm::printWarn("Parse error: $errmsg");
      YaFsm::printFatal("exiting script due to parse error");
    }
    else
    {
      $gFSMFileName = $filename;
      $gFSMName = basename($gFSMFileName,'.xml');

      $gFSM = $xmlContent;

      print Dumper($gFSM) if $YaFsm::gVerbose;
      YaFsm::printDbg("parsing Definitons (actions, timers, events) ");
      parseDefinitions($gFSM->{definitions});# if $YaFsm::gVerbose;
      YaFsm::printDbg("parsing FSM");
      if ($gFSMGenView)
      {
        # clean dot wallpaper filename
        my $outWallpaperFilePath = $gFSMViewOutPath . '/wallpaper.dot';
        open(FH,">$outWallpaperFilePath") || die "cannot open $outWallpaperFilePath for cleaning\n";
        print FH "digraph G {\n";
        print FH " compound=true;\n";
        close(FH);
      }

      parseFSM($gFSM->{fsm},"root");# if $YaFsm::gVerbose;

      YaFsm::printFatal("mismatch in state levels $gStateLevel") if $gStateLevel != -1;

      # after parsing, write out description file for OpemFSMViewer
      YaFsmViewerGen::writeDscFile()  if( $gFSMGenView);
      # write out code files
      YaFsmCodeGen::writeCodeFiles($gFSMName)  if( $gFSMGenCode);

      if ($gFSMGenView)
      {
        # clean dot wallpaper filename
        my $outWallpaperFilePath = $gFSMViewOutPath . '/wallpaper.dot';
        open(FH,">>$outWallpaperFilePath") || die "cannot open $outWallpaperFilePath for writing\n";
        print FH "}\n";
        close(FH);
        # write out wallpaper svg file
        YaFsmViewerGen::genDotTypeFile('wallpaper')  if( $gFSMGenView);
      }
    }


    if(!defined $gFSMFileName)
    {
      YaFsm::printFatal("fsm file $filename could not be read");
    }
  }
  else
  {
    YaFsm::printFatal("fsm file $filename not found");
  }
}

sub hasSubStates
{
  my $hasSubStates = 0;
  my $state = shift;
  if($state->{state})
  {
   	YaFsm::printDbg("Found substates in state $state->{name}");
    $hasSubStates = 1;
  }

  return $hasSubStates;
}


sub getMethodTokens
{
  my $str = shift;
  my $parseUsage = 0;
  $parseUsage = shift;
  my %func;

  my @tokens = split(/;/,$str);
  foreach my $token (@tokens)
  {
    $token =~ /(\b\w+\b).*\((.*)\)/;
    my $funcname = $1;
    my $args = $2;
    my @args = split(/,/,$args);

    foreach my $arg(@args)
    {
      my $argtype;
      my $argname;
      $arg =~ s/^\s+//;
      $arg =~ s/\s+$//;
      $arg =~ /(.+)(\s+)+(\b\w+\b)+/;
      if( $1 && $3 )
      {
        $argtype = $1;
        $argname = $3;
      }
      else
      {
        $arg =~ /(\b\w+\b)/;
        if( $parseUsage )
        {
          $argname = $1;
        }
        else
        {
          $argtype = $1;
        }
      }

      push(@{$func{params}},{name => $argname, type => $argtype});

    }
    $func{name}=$funcname;
  }
  return %func;
}

sub getEnterStateName
{
  my $state = shift;
  my $enterStateName;
  if($state->{state})
  {
    foreach my $substate (@{$state->{state}})
    {
      if ( $substate->{type} &&  "entry" eq $substate->{type} )
      {
        $enterStateName = $substate->{name};
      }
    }
  }

  return $enterStateName;
}

sub hasStateActions
{
  my $hasStateActions = 0;
  my $state = shift;
  if(defined $state->{enter}
     || defined $state->{exit}
     || defined $state->{tstartenter}
     || defined $state->{tstopenter}
     || defined $state->{tstartexit}
     || defined $state->{tstopexit}
    )
  {
   	YaFsm::printDbg("Found state enter/exit/timer actions in state $state->{name}");
    $hasStateActions = 1;
  }

  return $hasStateActions;

}

sub hasStateEnterActions
{
  my $hasStateActions = 0;
  my $state = shift;
  if(defined $state->{enter}
     || defined $state->{tstartenter}
     || defined $state->{tstopenter}
    )
  {
    $hasStateActions = 1;
  }

  return $hasStateActions;

}

sub hasStateExitActions
{
  my $hasStateActions = 0;
  my $state = shift;
  if(defined $state->{exit}
     || defined $state->{tstartexit}
     || defined $state->{tstopexit}
    )
  {
    $hasStateActions = 1;
  }

  return $hasStateActions;

}






sub hasTransitionActions
{
  my $hasTransActions = 0;
  my $trans = shift;
  if(defined $trans->{action})
  {
   	YaFsm::printDbg("Found transition actions in trigger $trans->{trigger}");
    $hasTransActions = 1;
  }

  return $hasTransActions;

}

sub hasTransitionEvents
{
  my $hasTransEvents = 0;
  my $trans = shift;
  if(defined $trans->{event})
  {
   	YaFsm::printDbg("Found transition event in trigger $trans->{trigger}");
    $hasTransEvents = 1;
  }

  return $hasTransEvents;

}


sub parseFSM
{
  my $currRef = shift;
  my $parentName = shift;
  my $parentParentName=shift;
  $gStateLevel++;

  foreach my $state (@{$currRef->{state}})
  {
    #my $cnt = keys(%{$cfg});
   	#YaFsm::printDbg("$cnt");
    YaFsm::printDbg("states on substate of $parentName: $state->{name}");
    YaFsm::printDbg("$parentName state level $gStateLevel");

    push(@gFSMStates, $state->{name});

    if(hasSubStates($state))
    {
      parseFSM($state, $state->{name}, $parentName);
    }
  }

  foreach my $trans (@{$currRef->{transition}})
  {
    if($trans->{trigger})
    {
      YaFsm::printDbg("transition on state $parentName: startstate $trans->{begin}, endstate $trans->{end}, trigger $trans->{trigger}");
    }
    else
    {
      YaFsm::printDbg("transition on state $parentName: trigger <empty>");
    }
  }


  YaFsmViewerGen::genDotFile($parentName,$currRef, $parentParentName) if ($gFSMGenView);
  YaFsmViewerGen::genDotTypeFile($parentName) if ($gFSMGenView);
  YaFsmViewerGen::genDscFile($parentName,$currRef, $gStateLevel) if ($gFSMGenView);
  #YaFsmCodeGen::genCodeFiles($parentName,$currRef,$parentParentName) if( $gFSMGenCode);


  $gStateLevel--;

}


sub parseDefinitions
{
  my $currRef = shift;

  foreach my $action (@{$currRef->{action}})
  {
    #my $cnt = keys(%{$cfg});
   	#YaFsm::printDbg("$cnt");
    if(!defined $action->{param})
    {
      $action->{param}="";
    }
    YaFsm::printDbg("action: $action->{name} ( $action->{param} )");
    $gFSMActions{$action->{name}}= $action->{param};
  }

  foreach my $trigger (@{$currRef->{trigger}})
  {
    #my $cnt = keys(%{$cfg});
   	#YaFsm::printDbg("$cnt");

    #todo
    # timer stuff are no triggers
    if(!defined $trigger->{param})
    {
      $trigger->{param}="";
    }
    YaFsm::printDbg("trigger: $trigger->{name} ( $trigger->{param} )");
    $gFSMTriggers{$trigger->{name}}= $trigger->{param};
  }

  foreach my $timer (@{$currRef->{timer}})
  {
    #my $cnt = keys(%{$cfg});
   	#YaFsm::printDbg("$cnt");
    my %timerData;
    if($timer->{cnt})
    {
      YaFsm::printDbg("timer:  $timer->{name}, timeout(ms): $timer->{ms}, repeat cnt: $timer->{cnt} ");
      $gFSMTimers{$timer->{name}}->{ms}= $timer->{ms};
      $gFSMTimers{$timer->{name}}->{cnt}= $timer->{cnt};
    }
    else
    {
      YaFsm::printDbg("timer:  $timer->{name}, timeout(ms): $timer->{ms}, single shot");
      $gFSMTimers{$timer->{name}}->{ms}= $timer->{ms};
      $gFSMTimers{$timer->{name}}->{cnt}= 1;
    }
    $gFSMTriggers{"timer$timer->{name}"}= "";
  }

  foreach my $event (@{$currRef->{event}})
  {
    #my $cnt = keys(%{$cfg});
   	#YaFsm::printDbg("$cnt");
    push(@gFSMEvents,$event->{name});
    YaFsm::printDbg("event:  $event->{name}");

    #auto generate triggers
    $gFSMTriggers{"event$event->{name}"}= "";

  }

  foreach my $member (@{$currRef->{member}})
  {
    #my $cnt = keys(%{$cfg});
    #YaFsm::printDbg("$cnt");
    YaFsm::printDbg("member:  $member->{type} $member->{name}, init $member->{init}");
    $gFSMMembers{$member->{name}}= { type => $member->{type}, init => $member->{init}} ;
  }

  foreach my $include (@{$currRef->{include}})
  {
    #my $cnt = keys(%{$cfg});
    #YaFsm::printDbg("$cnt");
    YaFsm::printDbg("include:  $include->{file}");
    push(@gFSMIncludes, $include->{file});
  }


}


1;
