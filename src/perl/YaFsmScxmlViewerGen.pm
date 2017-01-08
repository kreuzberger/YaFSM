package YaFsmScxmlViewerGen;

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
  $YaFsm::gVerbose = 1;
  eval "use Data::Dumper";

  my $basename = shift;
  my $filename = $basename;
  my $currRef = shift;
  my $parentName =shift;
  my $fh;
  my $fhWallpaper;

  $filename .= ".dot";
  my $outFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename;
  my $outWallpaperFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/wallpaper.dot';

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


    print $fhWallpaper "  $basename"."_entry [shape=point] ;\n";

    if(defined $parentName)
    {
      if($gUseFilePath)
      {
        print $fh "  $basename"."_entry [shape=point, URL=\"$YaFsmScxmlParser::gFSMViewOutPath/$parentName.html\"];\n";
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
    print Dumper($currRef);

    if ( $currRef->{initial} )
    {
      if( ref($currRef->{initial}) eq 'HASH'&& $currRef->{initial}{transition} )
      {
        if( ref($currRef->{initial}{transition}) eq 'ARRAY')
        {
          my $enterStateName = $currRef->{initial}{transition}[0]{target};
          print $fh "  $currRef->{id}"."_entry -> $enterStateName\n";
          print $fhWallpaper "  $currRef->{id}"."_entry -> $enterStateName\n";
        }
      }
      else
      {
        print $fh "  $basename"."_entry -> $currRef->{initial}\n";
        print $fhWallpaper "  $basename"."_entry -> $currRef->{initial}\n";
      }

      #eq 'HASH' && $currRef->{initial}{transition}
    }


    foreach my $state (@{$currRef->{state}})
    {
      if ( $state->{initial} && $state->{initial}{transition})
      {
   #     my $enterStateName = $state->{initial}{transition}[0]{target};
   #     print $fh "  $state->{id}"."_entry -> $enterStateName\n";
   #     print $fhWallpaper "  $state->{id}"."_entry -> $enterStateName\n";
      }

      if(YaFsmScxmlParser::hasStateActions($state))
      {
        print $fh "  $state->{id}  [shape=record, label=<{$state->{id}";
        print $fhWallpaper "  $state->{id}  [shape=record, label=<{$state->{id}";
        if(YaFsmScxmlParser::hasStateEnterActions($state))
        {
          print $fh "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fhWallpaper "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fh "<TR><TD>";
          print $fhWallpaper "<TR><TD>";

          if(defined $state->{onentry} && defined $state->{onentry}{script})
          {
            my @list = YaFsmScxmlParser::scriptCodeToArray($state->{onentry}{script});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/enter.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/enter.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/enter.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/enter.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
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
                #print $fh "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
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
                #print $fh "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
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

          if(defined $state->{evententer})
          {
            my @list = split(';',$state->{evententer});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/enter.$YaFsmScxmlParser::gImageExt\"/>^$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/enter.$YaFsmScxmlParser::gImageExt\"/>^$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                print $fh "^$_<br/>";
                print $fhWallpaper "^$_<br/>";
              }
            }
          }

          print $fh "</TD></TR>";
          print $fhWallpaper "</TD></TR>";
          print $fh "</TABLE></FONT>";
          print $fhWallpaper "</TABLE></FONT>";
        }
        if(YaFsmScxmlParser::hasStateExitActions($state))
        {
          print $fh "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fhWallpaper "|<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE BORDER=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fh "<TR><TD>";
          print $fhWallpaper "<TR><TD>";

          if(defined $state->{onexit} && defined $state->{onexit}{script})
          {
            my @list = YaFsmScxmlParser::scriptCodeToArray($state->{onexit}{script});

            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/exit.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/exit.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/exit.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/exit.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
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
                #print $fh "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
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
                #print $fh "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_/TD></TR>";
                print $fh "<IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/timerstop.$YaFsmScxmlParser::gImageExt\"/>$_<br/>";
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

          if(defined $state->{eventexit})
          {
            my @list = split(';',$state->{eventexit});
            foreach (@list)
            {
              if($gUseImages)
              {
                #print $fh "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                #print $fhWallpaper "<TR><TD><IMG SRC=\"images/timerstart.$YaFsmScxmlParser::gImageExt\"/></TD><TD>$_</TD></TR>";
                print $fh "<IMG SRC=\"images/exit.$YaFsmScxmlParser::gImageExt\"/>^$_<br/>";
                print $fhWallpaper "<IMG SRC=\"images/exit.$YaFsmScxmlParser::gImageExt\"/>^$_<br/>";
              }
              else
              {
                #print $fh "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                #print $fhWallpaper "<TR><TD>$gTStart&nbsp;timer$_</TD></TR>";
                print $fh "^$_<br/>";
                print $fhWallpaper "^$_<br/>";
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
        print $fh "  $state->{id} [label=\"$state->{id}\"";
        print $fhWallpaper "  $state->{id} [label=\"$state->{id}\"";
      }

      if(YaFsmScxmlParser::hasSubStates($state))
      {
        if($gUseFilePath)
        {
          print $fh "  color=grey, URL=\"$YaFsmScxmlParser::gFSMViewOutPath/$state->{id}.html\"";
        }
        else
        {
          print $fh "  color=grey, URL=\"$state->{id}.html\"";
        }
      }

      print $fh "];\n";
      print $fhWallpaper "];\n";
      if(YaFsmScxmlParser::hasSubStates($state))
      {
        print $fhWallpaper "  $state->{id} -> $state->{id}_entry [lhead=cluster".$state->{id}."]\n";
        #$strWallpaperStateLinks = "  $state->{id} -> $state->{id}_entry [lhead=cluster".$state->{id}."]\n";
      }

      foreach my $trans (@{$state->{transition}})
      {
        print $fh "  $trans->{source} -> $trans->{target} ";
        print $fhWallpaper "  $trans->{source} -> $trans->{target} ";
        if($trans->{event})
        {
          print $fh "[label=<<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fhWallpaper "[label=<<FONT POINT-SIZE=\"$gActionFontSize\"><TABLE border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
          print $fh "<TR><TD>" . YaFsm::xmlEscape($trans->{event});
          print $fhWallpaper "<TR><TD>" . YaFsm::xmlEscape($trans->{event});



          if ($YaFsmScxmlParser::gFSMTriggers{$trans->{event}})
          {
            print $fh "(" . YaFsm::xmlEscape($YaFsmScxmlParser::gFSMTriggers{$trans->{event}}) .")";
            print $fhWallpaper "(" . YaFsm::xmlEscape($YaFsmScxmlParser::gFSMTriggers{$trans->{event}}).")";
          }
          if($trans->{cond})
          {
            # add some extra space to get a better layout (egdge labes are not distored)
            print $fh " [" . YaFsm::xmlEscape($trans->{cond}) ."]     ";
            print $fhWallpaper " [" . YaFsm::xmlEscape($trans->{cond}) ."]     ";
          }
          print $fh "<br/>";
          print $fhWallpaper "<br/>";

          if(YaFsmScxmlParser::hasTransitionActions($trans))
          {

            if(defined $trans->{script} )
            {
              my @actionArray = YaFsmScxmlParser::scriptCodeToArray($trans->{script});

              print $fh "<FONT color=\"grey\">";
              print $fhWallpaper "<FONT color=\"grey\">";
              foreach(@actionArray)
              {
                print $fh "$_<br/>";
                print $fhWallpaper "$_<br/>";
              }
              print $fh "</FONT>";
              print $fhWallpaper "</FONT>";
            }
          }

          if(YaFsmScxmlParser::hasTransitionEvents($trans))
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
  #chdir($YaFsmScxmlParser::gFSMViewOutPath);
  my $outDotTypeFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.' . $YaFsmScxmlParser::gFSMGenDotType;
  my $outDotTypeFileImgPath;
  if( $gUseFilePath)
  {
    $outDotTypeFileImgPath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.' . $YaFsmScxmlParser::gFSMGenDotType;
  }
  else
  {
    $outDotTypeFileImgPath = $filename .'.' . $YaFsmScxmlParser::gFSMGenDotType;
  }

  my $outDotMapFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.map';
  my $outDotFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.dot';

  YaFsm::printDbg("writing $YaFsmScxmlParser::gFSMGenDotType file $outDotFilePath");

    #my $cnt = keys(%{$cfg});
    #YaFsm::printDbg("$cnt");
  my $dotCallStr= "dot -Tcmapx -o$outDotMapFilePath  -T".$YaFsmScxmlParser::gFSMGenDotType." -o $outDotTypeFilePath $outDotFilePath";

  YaFsm::printDbg("calling dot: $dotCallStr");
  system($dotCallStr);

  my $htmlfilename .= $filename . ".html";
  my $outFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $htmlfilename;

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
  my $strStates;
  my $strTransitions;


  #chdir($YaFsmScxmlParser::gFSMViewOutPath);
  #my $outDotTypeFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.' . $YaFsmScxmlParser::gFSMGenDotType;
  my $outDotHtmlFilePath;
  if($gUseFilePath)
  {
    $outDotHtmlFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.html';
  }
  else
  {
    $outDotHtmlFilePath = $filename .'.html';
  }
  #my $outDotHtmlFilePath = $filename .'.html';
  #my $outDotFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.dot';

  my $outFilePath;
  $filename .= ".txt";
  if($gUseFilePath)
  {
    $outFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename;
  }
  else
  {
    $outFilePath = $filename;
  }
  #my $outFilePath = $filename;

  YaFsm::printDbg("writing dsc file $filename on level $stateLevel");

  open ( $fh, ">$YaFsmScxmlParser::gFSMViewOutPath/$filename") or YaFsm::printFatal "cannot open file $YaFsmScxmlParser::gFSMViewOutPath/$filename for writing";


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

    $strStates .= "states;\n";
    foreach my $state (@{$currRef->{state}})
    {
      $strStates .= "  $state->{id};\n";
      if(YaFsmScxmlParser::hasStateActions($state))
      {
        $strStates .= "    enter;\n";
        if(YaFsmScxmlParser::hasStateEnterActions($state))
        {
          if(defined $state->{onenter} && defined $state->{onenter}{script})
          {
            my @actionArray = YaFsmScxmlParser::scriptCodeToArray($state->{onenter}{script});

            foreach(@actionArray)
            {
              $strStates .= "      script;$_\n" ;
            }
          }

          if(defined $state->{tstartenter})
          {
            my @actionArray= split(';',$state->{tstartenter});
            foreach(@actionArray)
            {
              $strStates .= "      startTimer;$_\n" ;
            }
          }

          if(defined $state->{tstopenter})
          {
            my @actionArray= split(';',$state->{tstopenter});
            foreach(@actionArray)
            {
              $strStates .= "      stopTimer;$_\n" ;
            }
          }

        }

        $strStates .= "    exit;\n";
        if(YaFsmScxmlParser::hasStateExitActions($state))
        {
          if(defined $state->{onexit} && defined $state->{onexit}{script})
          {
            my @actionArray = YaFsmScxmlParser::scriptCodeToArray($state->{onexit}{script});

            foreach(@actionArray)
            {
              $strStates .= "      script;$_\n" ;
            }
          }

          if(defined $state->{tstartexit})
          {
            my @actionArray= split(';',$state->{tstartexit});
            foreach(@actionArray)
            {
              $strStates .= "      startTimer;$_\n" ;
            }
          }

          if(defined $state->{tstopexit})
          {
            my @actionArray= split(';',$state->{tstopexit});
            foreach(@actionArray)
            {
              $strStates .= "      stopTimer;$_\n" ;
            }
          }

        }
      }

      foreach my $trans (@{$state->{transition}})
      {
        if($trans->{event})
        {
          $strTransitions .= "  $trans->{event};\n";
          $strTransitions .= "    source;$state->{id};\n";
          $strTransitions .= "    target;$trans->{target}\n";
          $strTransitions .= "    cond;$trans->{cond}\n" if($trans->{cond});
          if(defined YaFsmScxmlParser::hasTransitionActions($trans))
          {
            if(defined $trans->{script} )
            {
              my @actionArray = YaFsmScxmlParser::scriptCodeToArray($trans->{script});

              foreach(@actionArray)
              {
                $strTransitions .= "      script;$_\n" ;
              }
            }
          }
          $strTransitions .= "    event;$trans->{event}\n" if(YaFsmScxmlParser::hasTransitionEvents($trans));
        }
      }

    }

    print $fh $strStates;
    print $fh "transitions;\n";
    if( $strTransitions )
    {
      print $fh $strTransitions;
    }

    if( 0 == $stateLevel )
    {
      print $fh "timers;\n";
      foreach my $key (keys(%YaFsmScxmlParser::gFSMTimers))
      {
        YaFsm::printDbg("$key => ");
        print $fh "  $key;\n";
        print $fh "    timeout;$YaFsmScxmlParser::gFSMTimers{$key}{ms};\n";
        print $fh "    repeat;$YaFsmScxmlParser::gFSMTimers{$key}{cnt}\n";
      }

      print $fh "events;\n";
      foreach my $events (@YaFsmScxmlParser::gFSMEvents)
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
  return $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename .'.' .$gFSMParser::gFSMGenDotType;
}

sub writeDscFile
{
  #chdir($YaFsmScxmlParser::gFSMViewOutPath);

  my $filename = "index.txt";
  my $outFilePath = $YaFsmScxmlParser::gFSMViewOutPath . '/' . $filename;

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
