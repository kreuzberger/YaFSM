#!perl -w

use strict;
use warnings;

#use Getopt::Long qw(:config debug);
use Getopt::Long;
use Pod::Usage;

use YaFsm;
use YaFsmScxmlParser;

my $gFSMFileName;
my $gFSMGenView = 0;
my $gFSMGenDotType='svg';
my $gFSMViewOutPath;
my $gFSMGenCode = 0;
my $gFSMCodeOutPath;

my $optOk = GetOptions (
#            'rootpath=s' => \$ProBuild::RootPath,
 #           'debug' => sub{$ProBuild::currBuildCfg = 'debug'},
 #           'genmake!' => \$ProBuild::gCurrCfgRef->{'genmake'},
            'fsm=s' => \$gFSMFileName,
            'genview' => \$gFSMGenView,
            'dottype=s' => \$gFSMGenDotType,
            'outview=s' => \$gFSMViewOutPath,
            'gencode' => \$gFSMGenCode,
            'outcode=s' => \$gFSMCodeOutPath,
            'verbose' => \$YaFsm::gVerbose,
            'help' => sub{pod2usage(-verbose => 0);CORE::exit;},
            'man' => sub{pod2usage(-verbose => 1);CORE::exit;}
            );

if($optOk)
{
  # check if there are parameters to probuild that are not handled by getOpt
  # allowed is the name of a buildcfg file passed to probuild per drag and drop
  if ($ARGV[0])
  {
    YaFsm::printDbg("Unprocessed options available") ;
    foreach (@ARGV)
    {
      YaFsm::printDbg("$_");
    }
  }

  if(!defined $gFSMFileName)
  {
    YaFsm::printFatal("Missing definition of fsm file name, please use option --fsm");
  }
  else
  {
    $YaFsmScxmlParser::gFSMFileName = $gFSMFileName;
    $YaFsmScxmlParser::gFSMGenCode = $gFSMGenCode;
    $YaFsmScxmlParser::gFSMCodeOutPath = $gFSMCodeOutPath;

    $YaFsmScxmlParser::gFSMGenView = $gFSMGenView;
    $YaFsmScxmlParser::gFSMGenDotType = $gFSMGenDotType;
    $YaFsmScxmlParser::gFSMViewOutPath = $gFSMViewOutPath;

    YaFsmScxmlParser::init($gFSMFileName);
    YaFsmScxmlParser::readFSM($gFSMFileName);
  }

}

__END__

=head1 NAME

yafsm - FSM Parser and Code Generator

=head1 SYNOPSIS

yafsm [options]

 Options:
   --help             brief help message
   --man              detailed options description
   --verbose          verbose debug messages
   --fsm              fsm file name
   --genview          generate yafsmviewer files
   --dottype          generate images from dot files
   --outview          path for yafsmviewer generation
   --gencode          generate code
   --outcode          path for code generation

=head1 OPTIONS

=over 8

=item B<--help>

Print a brief help message and exits.

=item B<--man>

Print detailed options descriptions and exit.

=item B<--verbose>

Print detailled debug messages. Only important through debugging of probuild

=item B<--fsm>

FSM File name.
XML FSM File description.

=head1 DESCRIPTION

B<This program> generated and parses fsm file description

=cut

