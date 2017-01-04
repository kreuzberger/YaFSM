package YaFsmScxmlParser;

use strict;
use warnings;
use Cwd;
use File::Path;
use File::Basename;
use File::Copy;
use YaFsmCodeGen;
use FindBin;




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
our $gFSMGenCode=0;
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
  my $fsmbasename=basename($fsmname,'.scxml');
  if((!defined $gFSMCodeOutPath))
  {
    $gFSMCodeOutPath= cwd() . '/' .lc($fsmbasename).'/code';
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

    if( $filename =~ m/.*\.scxml/)
    {

      # read XML file
      # force array is useful for configurations that have only one entry and are not parsed into
      # array by default. So we ensure that the buildcfg always is an array!
      #  my $xmlContent = eval{$xml->XMLin("$filename", SuppressEmpty => '',ForceArray => qr/buildcfg$/)};
      my $xmlContent = eval{$xml->XMLin("$filename", ForceArray => [qw(state transition final data)])};
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
        $gFSMName = basename($gFSMFileName,'.scxml');

        $gFSM = $xmlContent;

        print Dumper($gFSM) if $YaFsm::gVerbose;
        YaFsm::printDbg("parsing Definitons (datamodel) ");
        parseDefinitions($gFSM);# if $YaFsm::gVerbose;
        YaFsm::printDbg("parsing FSM");


        parseFSM($gFSM,"root");# if $YaFsm::gVerbose;

        YaFsm::printFatal("mismatch in state levels $gStateLevel") if $gStateLevel != -1;

        # write out code files
        YaFsmCodeGen::writeCodeFiles($gFSMName, 1)  if( $gFSMGenCode);
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
     YaFsm::printDbg("Found substates in state $state->{id}") if ($state->{id});
    $hasSubStates = 1;
  }
  elsif($state->{final})
  {
    YaFsm::printDbg("Found final substate in state $state->{id}");
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
  if(defined $state->{onentry}
     || defined $state->{onexit}
    )
  {
    YaFsm::printDbg("Found state enter/exit/ actions in state $state->{id}");
    $hasStateActions = 1;
  }

  return $hasStateActions;

}

sub hasStateEnterActions
{
  my $hasStateActions = 0;
  my $state = shift;
  if(defined $state->{onentry}
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
  if(defined $state->{onexit}
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
  if(defined $trans->{script}
     || defined $trans->{assign}
  )
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
    YaFsm::printDbg("states on substate of $parentName: $state->{id}");
    YaFsm::printDbg("$parentName state level $gStateLevel");

    push(@gFSMStates, $state->{id});

    if(hasSubStates($state))
    {
      parseFSM($state, $state->{id}, $parentName);
    }

    foreach my $trans (@{$currRef->{transition}})
    {
      if($trans->{trigger})
      {
        YaFsm::printDbg("transition on state $parentName: startstate $state->{id}, endstate $trans->{target}, trigger $trans->{event}");

      }
      else
      {
        YaFsm::printDbg("transition on state $parentName: trigger <empty>");
      }
      $trans->{source} = $state->{id};
    }

  }


  $gStateLevel--;

}

sub parseScxmlFSM
{
  my $currRef = shift;
  my $parentName = shift;
  my $parentParentName=shift;
  $gStateLevel++;


  foreach my $state (@{$currRef->{state}})
  {
    YaFsm::printDbg("states on substate of $parentName: $state->{id}");
    YaFsm::printDbg("$parentName state level $gStateLevel");

    push(@gFSMStates, $state->{id});

    if(hasSubStates($state))
    {
      parseScxmlFSM($state, $state->{id}, $parentName);
    }

    foreach my $trans (@{$currRef->{transition}})
    {
      if($trans->{event})
      {
        YaFsm::printDbg("transition on state  $state->{id}: startstate $state->{id}, endstate $trans->{target}, trigger $trans->{event}");
      }
      else
      {
        YaFsm::printDbg("transition on state $state->{id}: trigger <empty>");
      }
    }
  }

  foreach my $state (@{$currRef->{final}})
  {
    YaFsm::printDbg("states on substate of $parentName: $state->{id}");
    YaFsm::printDbg("$parentName state level $gStateLevel");

    push(@gFSMStates, $state->{id});

    if(hasSubStates($state))
    {
      parseScxmlFSM($state, $state->{id}, $parentName);
    }

    foreach my $trans (@{$currRef->{transition}})
    {
      if($trans->{event})
      {
        YaFsm::printDbg("transition on state  $state->{id}: startstate $state->{id}, endstate $trans->{target}, trigger $trans->{event}");
      }
      else
      {
        YaFsm::printDbg("transition on state  $state->{id}: trigger <empty>");
      }
    }
  }


  $gStateLevel--;

}



sub parseDefinitions
{
  my $currRef = shift;

  foreach my $data (@{$currRef->{datamodel}})
  {
    YaFsm::printDbg("found data element");
  }


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
