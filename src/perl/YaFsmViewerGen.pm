package YaFsmViewerGen;

use strict;
use warnings;
use File::Path;
use File::Basename;



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

my @gOutStateList;
my $gActionFontSize='7';
my $gStateFontSize='8';
my $gFontName='Courier New';
#my $gRArrow='&#8658;';
#my $gLArrow='&#8656;';
my $gRArrow='=&gt;';
my $gLArrow='&lt;=';
#my $gTStart='&#8853;';
#my $gTStop='&#8855;';
my $gTStart='+';
my $gTStop='-';
my $gUseImages=0;
my $gUseFilePath=0;


sub setDefaultOpt
{
#  my %newBuildCfg = ( buildcfg => [{name => "",uninstall => 0 }]);
#  my @newBuildCfg = genCfg();
#  my $newBuildCfg = \%newBuildCfg;

#  push (@{$buildCfg{buildcfg}},@newBuildCfg);
#  $buildCfgs =\%buildCfg;
#  $buildCfgsCnt = @{$buildCfgs->{buildcfg}};
#  $gCurrCfgRef = \%{$buildCfgs->{buildcfg}->[0]};
}

#dot wallpaper example
# digraph G {
# compound=true;
#subgraph cluster0 {
#a
#}
#subgraph cluster1{
#b
#}
#a -> b [lhead=cluster1]
#}

sub genDotFile
{
  my $basename = shift;
  my $filename = $basename;
  my $currRef = shift;
  my $parentName =shift;
  my $fh;
  my $fhWallpaper;

  $filename .= ".dot";
  my $outFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename;
  my $outWallpaperFilePath = $YaFsmParser::gFSMViewOutPath . '/wallpaper.dot';

  YaFsm::printDbg("writing dot file $filename");
  open ( $fhWallpaper, ">>$outWallpaperFilePath") or YaFsm::printFatal "cannot open file $outWallpaperFilePath for appending";
  open ( $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";

  if($fh && $fhWallpaper)
  {
    print $fh "digraph G {\n";
#    if(defined $parentName)
    {
 #     print $fhWallpaper "subgraph cluster".$parentName." {\n";
      print $fhWallpaper " subgraph cluster".$basename." {\n";
      print $fhWallpaper "  label = $basename\n";
    }

    print $fh "  node [";
    print $fhWallpaper "  node [";
    #print $fh " fontname = \"Bitstream Vera Sans\"\n";
    print $fh " fontname = \"$gFontName\",";
    print $fhWallpaper " fontname = \"$gFontName\",";
#    print $fh " shape = \"record\"\n";
    print $fh " fontsize=$gStateFontSize,";
    print $fhWallpaper " fontsize=$gStateFontSize,";
    print $fh " shape=\"box\",";
    print $fhWallpaper " shape=\"box\",";
    print $fh " style=\"rounded\"";
    print $fhWallpaper " style=\"rounded\"";
    print $fh " ]\n";
    print $fhWallpaper " ]\n";

    print $fh "  edge [";
    print $fhWallpaper "  edge [";
    print $fh " fontname = \"$gFontName\",";
    print $fhWallpaper " fontname = \"$gFontName\",";
    print $fh " fontsize=$gActionFontSize";
    print $fhWallpaper " fontsize=$gActionFontSize";
    #print $fh " fontname = \"Bitstream Vera Sans\"\n";
    #print $fh " decorate=true\n";
    #print $fhWallpaper " decorate=true\n";

    print $fh " ]\n";
    print $fhWallpaper " ]\n";
    foreach my $trans (@{$currRef->{transition}})
    {
      print $fh "  $trans->{begin} -> $trans->{end} ";
      print $fhWallpaper "  $trans->{begin} -> $trans->{end} ";
      if($trans->{trigger})
      {
        print $fh "[label=<<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        print $fhWallpaper "[label=<<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        print $fh "<TR><TD>" . YaFsm::xmlEscape($trans->{trigger});
        print $fhWallpaper "<TR><TD>" . YaFsm::xmlEscape($trans->{trigger});



        if ($YaFsmParser::gFSMTriggers{$trans->{trigger}})
        {
          print $fh "(" . YaFsm::xmlEscape($YaFsmParser::gFSMTriggers{$trans->{trigger}}) .")";
          print $fhWallpaper "(" . YaFsm::xmlEscape($YaFsmParser::gFSMTriggers{$trans->{trigger}}).")";
        }
        if($trans->{condition})
        {
          # add some extra space to get a better layout (egdge labes are not distored)
          print $fh " [" . YaFsm::xmlEscape($trans->{condition}) ."]     ";
          print $fhWallpaper " [" . YaFsm::xmlEscape($trans->{condition}) ."]     ";
        }
        print $fh "<br/>";
        print $fhWallpaper "<br/>";

        if(YaFsmParser::hasTransitionActions($trans))
        {
          if(length($trans->{action}))
          {
            my @actionArray = split(/;/,$trans->{action});
            print $fh "<FONT color=\"grey\">";
            print $fhWallpaper "<FONT color=\"grey\">";
            foreach(@actionArray)
            {
              print $fh "$_<br/>";
              print $fhWallpaper "$_<br/>";
              #print $fh "$trans->{param}" if($trans->{param});
              #print $fhWallpaper "$trans->{param}" if($trans->{param});
            }
            print $fh "</FONT>";
            print $fhWallpaper "</FONT>";
          }
        }

        if(YaFsmParser::hasTransitionEvents($trans))
        {
          if(length($trans->{event}))
          {
#            print $fh "<TR><TD><FONT color=\"grey\">^$trans->{event}</FONT></TD></TR>;";
#            print $fhWallpaper "<TR><TD><FONT color=\"grey\">^$trans->{event}</FONT></TD></TR>;";
            my @eventArray = split(/;/,$trans->{event});
            print $fh "<FONT color=\"grey\">";
            print $fhWallpaper "<FONT color=\"grey\">";
            foreach(@eventArray)
            {
              print $fh "^$_<br/>";
              print $fhWallpaper "^$_<br/>";
              #print $fh "$trans->{param}" if($trans->{param});
              #print $fhWallpaper "$trans->{param}" if($trans->{param});
            }
            print $fh "</FONT>";
            print $fhWallpaper "</FONT>";

          }
        }
        print $fh "</TD></TR>";
        print $fhWallpaper "</TD></TR>";
        print $fh "</TABLE></FONT>>];\n";
        print $fhWallpaper "</TABLE></FONT>>];\n";
      }
    }

    print $fhWallpaper "  $basename"."_entry [shape=point] ;\n";

    if(defined $parentName)
    {
      if($gUseFilePath)
      {
        print $fh "  $basename"."_entry [shape=point, URL=\"$YaFsmParser::gFSMViewOutPath/$parentName.html\"];\n";
      }
      else
      {
        print $fh "  $basename"."_entry [shape=point, URL=\"$parentName.html\"];\n";
      }
    }
    else
    {
      print $fh "  $basename"."_entry [shape=point] ;\n";
    }

    my $strWallpaperStateLinks;
    foreach my $state (@{$currRef->{state}})
    {
      #YaFsm::printDbg("states on level $level, substate of $parentName: $state->{name}");
      #check if the states has substates
      if ( $state->{type} && "entry" eq $state->{type} )
      {
          print $fh "  $basename"."_entry -> $state->{name}\n";
          print $fhWallpaper "  $basename"."_entry -> $state->{name}\n";
      }

      if(YaFsmParser::hasStateActions($state))
      {
        print $fh "  $state->{name}  [shape=record, label=<{$state->{name}";
        print $fhWallpaper "  $state->{name}  [shape=record, label=<{$state->{name}";
        if(YaFsmParser::hasStateEnterActions($state))
        {
          print $fh "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fhWallpaper "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fh "<TR><TD>";
          print $fhWallpaper "<TR><TD>";

          if(defined $state->{enter})
          {
            my @list = split(';',$state->{enter});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/enter.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/enter.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/enter.$YaFsmParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/enter.$YaFsmParser::gImageExt\"/>$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gRArrow&nbsp;$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gRArrow&nbsp;$_</TD></TR>";
                print $fh "$gRArrow&nbsp;$_<br/>";
                print $fhWallpaper "$gRArrow&nbsp;$_<br/>";
              }
            }
          }
          if(defined $state->{tstartenter})
          {
            my @list = split(';',$state->{tstartenter});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/>$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                print $fh "$gTStart&nbsp;timer$_<br/>";
                print $fhWallpaper "$gTStart&nbsp;timer$_<br/>";
              }
            }
          }
          if(defined $state->{tstopenter})
          {
            my @list = split(';',$state->{tstopenter});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/>$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gTStop&nbsp;timer$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gTStop&nbsp;timer$_</TD></TR>";
                print $fh "$gTStop&nbsp;timer$_<br/>";
                print $fhWallpaper "$gTStop&nbsp;timer$_<br/>";
              }
            }
          }
          print $fh "</TD></TR>";
          print $fhWallpaper "</TD></TR>";
          print $fh "</TABLE></FONT>";
          print $fhWallpaper "</TABLE></FONT>";
        }
        if(YaFsmParser::hasStateExitActions($state))
        {
          print $fh "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fhWallpaper "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fh "<TR><TD>";
          print $fhWallpaper "<TR><TD>";

          if(defined $state->{exit})
          {
            my @list = split(';',$state->{exit});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/exit.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/exit.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/exit.$YaFsmParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/exit.$YaFsmParser::gImageExt\"/>$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gLArrow&nbsp;$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gLArrow&nbsp;$_</TD></TR>";
                print $fh "$gLArrow&nbsp;$_<br/>";
                print $fhWallpaper "$gLArrow&nbsp;$_<br/>";
              }
            }
          }
          if(defined $state->{tstartexit})
          {
            my @list = split(';',$state->{tstartexit});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstart.$YaFsmParser::gImageExt\"/>$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                print $fh "$gTStart&nbsp;timer$_<br/>";
                print $fhWallpaper "$gTStart&nbsp;timer$_<br/>";
              }
            }
          }
          if(defined $state->{tstopexit})
          {
            my @list = split(';',$state->{tstopexit});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/></TD><TD>$_/TD></TR>";
                print $fh "<IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstop.$YaFsmParser::gImageExt\"/>$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gTStop&nbsp;timer$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gTStop&nbsp;timer$_</TD></TR>";
                print $fh "$gTStop&nbsp;timer$_<br/>";
                print $fhWallpaper "$gTStop&nbsp;timer$_<br/>";
              }
            }
          }
          print $fh "</TD></TR>";
          print $fhWallpaper "</TD></TR>";
          print $fh "</TABLE></FONT>";
          print $fhWallpaper "</TABLE></FONT>";

        }
        print $fh "}>";
        print $fhWallpaper "}>";

      }
      else
      {
        print $fh "  $state->{name} [label=\"$state->{name}\"";
        print $fhWallpaper "  $state->{name} [label=\"$state->{name}\"";
      }

      if(YaFsmParser::hasSubStates($state))
      {
        if($gUseFilePath)
        {
          print $fh "  color=grey, URL=\"$YaFsmParser::gFSMViewOutPath/$state->{name}.html\"";
        }
        else
        {
          print $fh "  color=grey, URL=\"$state->{name}.html\"";
        }
      }

      print $fh "];\n";
      print $fhWallpaper "];\n";
      if(YaFsmParser::hasSubStates($state))
      {
        print $fhWallpaper "  $state->{name} -> $state->{name}_entry [lhead=cluster".$state->{name}."]\n";
        #$strWallpaperStateLinks = "  $state->{name} -> $state->{name}_entry [lhead=cluster".$state->{name}."]\n";
      }

    }


    print $fh " }\n";
    print $fhWallpaper " }\n";

    close($fh);
    close($fhWallpaper);
  }
}

sub genDotTypeFile
{
  my $filename = shift;
  #chdir($YaFsmParser::gFSMViewOutPath);
  my $outDotTypeFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.' . $YaFsmParser::gFSMGenDotType;
  my $outDotTypeFileImgPath;
  if( $gUseFilePath)
  {
    $outDotTypeFileImgPath = $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.' . $YaFsmParser::gFSMGenDotType;
  }
  else
  {
    $outDotTypeFileImgPath = $filename .'.' . $YaFsmParser::gFSMGenDotType;
  }
  
  my $outDotMapFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.map';
  my $outDotFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.dot';

  YaFsm::printDbg("writing $YaFsmParser::gFSMGenDotType file $outDotFilePath");

    #my $cnt = keys(%{$cfg});
    #YaFsm::printDbg("$cnt");
  my $dotCallStr= "dot -Tcmapx -o$outDotMapFilePath  -T".$YaFsmParser::gFSMGenDotType." -o $outDotTypeFilePath $outDotFilePath";

  YaFsm::printDbg("calling dot: $dotCallStr");
  system($dotCallStr);

  my $htmlfilename .= $filename . ".html";
  my $outFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $htmlfilename;

  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";
  if($fh)
  {
    print $fh "<html>\n<body>\n";
    print $fh "<img src=\"$outDotTypeFileImgPath\" usemap=\"#G\">\n";
    #print $fh "<object data=\"$outDotTypeFilePath\" type=\"image/svg+xml\" width=\"100%\" height=\"100%\" usemap=\"#G\"/>\n";
    open ( my $fhMap, "<$outDotMapFilePath") or YaFsm::printFatal "cannot open file $outDotMapFilePath for reading";
    my @fhMap;
    if($fhMap)
    {
      @fhMap=<$fhMap>;
      foreach(@fhMap)
      {
        print $fh $_;
      }
      close($fhMap);
    }

    print $fh "</body>\n</html>\n";


    close($fh);
  }

}

sub genDscFile
{
  my $filename = shift;
  my $currRef = shift;
  my $stateLevel = shift;
  my $fh;
  my $fileBasename=$filename;


  #chdir($YaFsmParser::gFSMViewOutPath);
  #my $outDotTypeFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.' . $YaFsmParser::gFSMGenDotType;
  my $outDotHtmlFilePath;
  if($gUseFilePath)
  {
    $outDotHtmlFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.html';
  }
  else
  {
    $outDotHtmlFilePath = $filename .'.html';
  }
  #my $outDotHtmlFilePath = $filename .'.html';
  #my $outDotFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.dot';

  my $outFilePath;
  $filename .= ".txt";
  if($gUseFilePath)
  {
    $outFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename;
  }
  else
  {
    $outFilePath = $filename;
  }
  #my $outFilePath = $filename;

  YaFsm::printDbg("writing dsc file $filename on level $stateLevel");

  open ( $fh, ">$YaFsmParser::gFSMViewOutPath/$filename") or YaFsm::printFatal "cannot open file $YaFsmParser::gFSMViewOutPath/$filename for writing";


  my $stateDscRow =  ' ' x ($stateLevel*2) . "$fileBasename;$outDotHtmlFilePath;$outFilePath";
  YaFsm::printDbg("adding stateDscRow $stateDscRow");
  push(@gOutStateList,$stateDscRow);


  if($fh)
  {
    #print $fh "#YaFsmViewer description file for state $filename\n";

#states;
#  start;
#    enter;
#      doAction1;doit
#      startTimer1;
#    exit;
#      stopTimer1;
#  end;
#    enter;
#    exit;

    print $fh "states;\n";
    foreach my $state (@{$currRef->{state}})
    {
      print $fh "  $state->{name};\n";
      if(YaFsmParser::hasStateActions($state))
      {
        print $fh "    enter;\n";
        if(YaFsmParser::hasStateEnterActions($state))
        {
          if(defined $state->{enter})
          {
            my @actionArray= split(';',$state->{enter});
            foreach(@actionArray)
            {
              print $fh "      action;$_\n" ;
            }
          }

          if(defined $state->{tstartenter})
          {
            my @actionArray= split(';',$state->{tstartenter});
            foreach(@actionArray)
            {
              print $fh "      startTimer;$_\n" ;
            }
          }

          if(defined $state->{tstopenter})
          {
            my @actionArray= split(';',$state->{tstopenter});
            foreach(@actionArray)
            {
              print $fh "      stopTimer;$_\n" ;
            }
          }

        }

        print $fh "    exit;\n";
        if(YaFsmParser::hasStateExitActions($state))
        {
          if(defined $state->{exit})
          {
            my @actionArray= split(';',$state->{exit});
            foreach(@actionArray)
            {
              print $fh "      action;$_\n" ;
            }
          }

          if(defined $state->{tstartexit})
          {
            my @actionArray= split(';',$state->{tstartexit});
            foreach(@actionArray)
            {
              print $fh "      startTimer;$_\n" ;
            }
          }

          if(defined $state->{tstopexit})
          {
            my @actionArray= split(';',$state->{tstopexit});
            foreach(@actionArray)
            {
              print $fh "      stopTimer;$_\n" ;
            }
          }

        }
      }
    }

#transitions;
#  start;
#    begin;start;
#    end;end;
#    parameter;argc,argv
#    condition;argc==1 && argv != 0
#    action;
#      dosomething;
#        parameter;argc;
#    event;1;
#  end;
#    begin;start;
#    end;end;
#    parameter;
#    condition;
#    action;
#    event;

    print $fh "transitions;\n";
    foreach my $trans (@{$currRef->{transition}})
    {
      if($trans->{trigger})
      {
        print $fh "  $trans->{trigger};\n";
        print $fh "    begin;$trans->{begin};\n";
        print $fh "    end;$trans->{end}\n";
        print $fh "    parameter;$trans->{param}\n" if($trans->{param});
        print $fh "    condition;$trans->{condition}\n" if($trans->{condition});
        if(defined YaFsmParser::hasTransitionActions($trans))
        {
          if(defined $trans->{action})
          {
            my @actionArray= split(';',$trans->{action});
            foreach(@actionArray)
            {
              print $fh "      action;$_\n" ;
            }
          }
        }
        print $fh "    event;$trans->{event}\n" if(YaFsmParser::hasTransitionEvents($trans));
      }
    }

    if( 0 == $stateLevel )
    {
      print $fh "timers;\n";
      foreach my $key (keys(%YaFsmParser::gFSMTimers))
      {
        YaFsm::printDbg("$key => ");
        print $fh "  $key;\n";
        print $fh "    timeout;$YaFsmParser::gFSMTimers{$key}{ms};\n";
        print $fh "    repeat;$YaFsmParser::gFSMTimers{$key}{cnt}\n";
      }

      print $fh "events;\n";
      foreach my $events (@YaFsmParser::gFSMEvents)
      {
        print $fh "  EVENT_$events;\n";
      }


    }

    close($fh);
  }
}


sub getDotTypeFilePath
{
  my $filename = shift;
  return $YaFsmParser::gFSMViewOutPath . '/' . $filename .'.' .$gFSMParser::gFSMGenDotType;
}

sub writeDscFile
{
  #chdir($YaFsmParser::gFSMViewOutPath);

  my $filename = "index.txt";
  my $outFilePath = $YaFsmParser::gFSMViewOutPath . '/' . $filename;

  YaFsm::printDbg("writing dsc index file $filename");

  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";


  if($fh)
  {
    #foreach my $dsc (@gOutStateList)
    for(my $idx=$#gOutStateList; $idx >=0;$idx--)
    {
      print $fh " $gOutStateList[$idx]\n";
    }
    close($fh);
  }
}

1;
