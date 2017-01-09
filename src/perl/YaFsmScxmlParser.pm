package YaFsmScxmlParser;

use strict;
use warnings;
use Cwd;
use File::Path;
use File::Basename;
use File::Copy;
use YaFsmCodeGen;
use YaFsmScxmlViewerGen;
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

our $gFSMGenView=0;
our $gFSMGenDotType='svg';
our $gFSMViewOutPath;

our %gFSMActions;
our %gFSMTriggers;
our @gFSMStates;
our %gFSMEvents;
our $gFSM;
our $gFSMName;
our %gFSMMembers;
our @gFSMIncludes;
our $gImageExt="png";

# for scxml
our %gFSMDataModel;


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

#  $YaFsm::gVerbose=1;
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
      my $xmlContent = eval{$xml->XMLin("$filename", ForceArray => [qw(state transition final datamodel data raise send param )])};
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
        if ($gFSMGenView)
        {
          # clean dot wallpaper filename
          my $outWallpaperFilePath = $gFSMViewOutPath . '/wallpaper.dot';
          open(FH,">$outWallpaperFilePath") || die "cannot open $outWallpaperFilePath for cleaning\n";
          print FH "digraph G {\n";
          print FH " compound=true;\n";
          close(FH);
        }

        parseFSM($gFSM,"root");# if $YaFsm::gVerbose;

        YaFsm::printFatal("mismatch in state levels $gStateLevel") if $gStateLevel != -1;

        YaFsmScxmlViewerGen::writeDscFile()  if( $gFSMGenView);
        # write out code files
        YaFsmCodeGen::writeCodeFiles($gFSMName, 1)  if( $gFSMGenCode);


        if ($gFSMGenView)
        {
          # finalize dot wallpaper filename
          my $outWallpaperFilePath = $gFSMViewOutPath . '/wallpaper.dot';
          open(FH,">>$outWallpaperFilePath") || die "cannot open $outWallpaperFilePath for writing\n";
          print FH "}\n";
          close(FH);
          # write out wallpaper svg file
          YaFsmScxmlViewerGen::genDotTypeFile('wallpaper')  if( $gFSMGenView);
        }
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

  if($state->{initial})
  {
    if ( $state->{initial} && $state->{initial}{transition})
    {
      $enterStateName = $state->{initial}{transition}[0]{target};
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
    YaFsm::printDbg("Found transition actions in trigger $trans->{event}");
    $hasTransActions = 1;
  }

  return $hasTransActions;

}

sub hasTransitionEvents
{
  my $hasTransEvents = 0;
  my $trans = shift;
  if(defined $trans->{send} || defined $trans->{raise})
  {
    YaFsm::printDbg("Found transition event in trigger $trans->{send}") if $trans->{send};
    YaFsm::printDbg("Found transition event in trigger $trans->{raise}") if $trans->{raise};
    $hasTransEvents = 1;
  }

  return $hasTransEvents;

}

sub scriptCodeToArray
{
  my $strCode = shift;
  my @codeArray;

  my @str = split(/;/,$strCode);
  foreach(@str)
  {
    # remove newlines an whitespaces
    my $str = $_;
    $str =~s/^\s+|\s+$//g;
    $str =~ s/\R//g;
    push(@codeArray,"$str;") if ( 0 < length($str) );
  }

  return @codeArray;
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

    my $transIdx = 0;

    foreach my $trans (@{$state->{transition}})
    {
      if($trans->{event})
      {
        YaFsm::printDbg("transition on state $parentName: startstate $state->{id}, endstate $trans->{target}, trigger $trans->{event}");
        YaFsm::printDbg("trigger: $trans->{event} ()");
        $trans->{param} = "";
        $gFSMTriggers{$trans->{event}}= $trans->{param};
      }
      else
      {
        YaFsm::printDbg("transition on state $parentName: trigger <empty>");
      }
      $trans->{source} = $state->{id};
      if(%gFSMDataModel && $trans->{script})
      {
        push(@{$gFSMActions{$state->{id}}}, { type => "transition", name => "transition_$state->{id}_$trans->{event}_$transIdx", script => scriptCodeToArray($trans->{script}), source => $state->{id}, event => $trans->{event} } )  ;
      }
      if (  $trans->{raise} )
      {
        foreach(@{$trans->{raise}})
        {
          $gFSMEvents{$_->{event}}= {event => $_->{event}, delay => 0};
        }
      }
      if (  $trans->{send} )
      {
        foreach(@{$trans->{send}})
        {
          my $delay = 0;
          $delay = $_->{delay} if ($_->{delay} );
          $gFSMEvents{$_->{event}}= {event => $_->{event}, delay => $delay};
        }
      }


      $transIdx++;

    }

    if($state->{onentry})
    {
      if( %gFSMDataModel && $state->{onentry}{script} )
      {
        push(@{$gFSMActions{$state->{id}}}, { type => "onentry", name => "$state->{id}_onEntry", script => scriptCodeToArray($state->{onentry}{script}), source => $state->{id} } )  ;
      }
      if ( $state->{onentry}{raise} )
      {
        foreach (@{$state->{onentry}{raise}})
        {
          $gFSMEvents{$_->{event}} = {event => $_->{event}, delay => 0};
        }
      }
      if (  $state->{onentry}{send} )
      {
        foreach (@{$state->{onentry}{send}})
        {
          my $delay = 0;
          $delay = $_->{delay} if ($_->{delay} );
          my @param;
          @param = $_->{param} if ( $_->{param} );
          $gFSMEvents{$_->{event}}= {event => $_->{event}, delay => $delay, param => @param};
        }
      }
    }

    if($state->{onexit})
    {
      if( %gFSMDataModel && $state->{onexit}{script} )
      {
        push(@{$gFSMActions{$state->{id}}}, { type => "onexit", name => "$state->{id}_onExit", script => scriptCodeToArray($state->{onexit}{script}), source => $state->{id} } )  ;
      }
      if ( $state->{onexit}{raise} )
      {
        foreach($state->{onexit}{raise})
        {
          $gFSMEvents{$_->{event}} = {event => $_->{event}, delay => 0};
        }
      }
      if (  $state->{onexit}{send} )
      {
        foreach (@{$state->{onexit}{send}})
        {
          my $delay = 0;
          $delay = $_->{delay} if ($_->{delay} );
          my @param;
          @param = $_->{param} if ( $_->{param} );
          $gFSMEvents{$_->{event}}= {event => $_->{event}, delay => $delay, param => @param};
        }
      }

    }


    if(hasSubStates($state))
    {
      parseFSM($state, $state->{id}, $parentName);
    }

  }

  YaFsmScxmlViewerGen::genDotFile($parentName,$currRef, $parentParentName) if ($gFSMGenView);
  YaFsmScxmlViewerGen::genDotTypeFile($parentName) if ($gFSMGenView);
  YaFsmScxmlViewerGen::genDscFile($parentName,$currRef, $gStateLevel) if ($gFSMGenView);


  $gStateLevel--;

}


sub parseDefinitions
{
  my $currRef = shift;


  if( $currRef->{datamodel} )
  {
    my $strDataModel;
    if(ref($currRef->{datamodel}) eq 'ARRAY')
    {
      YaFsm::printDbg("data model string $currRef->{datamodel}[0]");
      $strDataModel = $currRef->{datamodel}[0];
    }
    else
    {
      YaFsm::printDbg("data model string $currRef->{datamodel}");
      $strDataModel = $currRef->{datamodel};
    }

    my @modelInfo = split(/:/,$strDataModel);
    YaFsm::printDbg(@modelInfo);

    if($#modelInfo == 2 )
    {
      if ("cplusplus" eq $modelInfo[0])
      {
        $gFSMDataModel{type}=$modelInfo[0];
        $gFSMDataModel{classname}=$modelInfo[1];
        $gFSMDataModel{headerfile}=$modelInfo[2];
      }
      else
      {
        YaFsm::printFatal("invalid datamodel type $modelInfo[0]");
      }

    }
    else
    {
      YaFsm::printFatal("invalid datamodel definition string $strDataModel");
    }


    # check for data members
    if(ref($currRef->{datamodel}) eq 'ARRAY')
    {
      YaFsm::printDbg("data model string $currRef->{datamodel}[1]");
      foreach my $data (@{$currRef->{datamodel}[1]{data}})
      {
        #YaFsm::printDbg("data:  $data->{id} $data->{expr}");
        if ( $data->{expr} )
        {
          $gFSMMembers{$data->{id}}= { expr => $data->{expr}} ;
        }
        elsif ($data->{src})
        {
          my @memberInfo = split(/:/,$data->{src});

          if($#memberInfo == 1 )
          {
            $gFSMMembers{$data->{id}}= { classname => $memberInfo[0], src => $memberInfo[1] } ;
          }
        }
      }
    }
  }
}


1;
