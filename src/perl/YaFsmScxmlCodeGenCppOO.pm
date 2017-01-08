package YaFsmScxmlCodeGenCppOO;

use strict;

require Exporter;
use vars       qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS);

# set the version for version checking
$VERSION     = 0.1;

@ISA         = qw(Exporter);
@EXPORT      = qw(&hello);
%EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],

# your exported package globals go here,
# as well as any optionally exported functions
@EXPORT_OK   = qw($Var1 %Hashit &func3);

use vars qw($Var1 %Hashit);
# non-exported package globals go here
use vars qw(@more $stuff);

# initialize package globals, first exported ones
$Var1   = '';
%Hashit = ();

# then the others (which are still accessible as $Some::Module::stuff)
$stuff  = '';
@more   = ();


# file-private lexicals go here
my $priv_var    = '';
my %secret_hash = ();


if($YaFsm::gVerbose)
{
  eval "use Data::Dumper";
  if($@)
  {
    YaFsm::printWarn $@;
    YaFsm::printFatal("Missing required package Data::Dumper");
  }
}

sub writeCodeFiles
{
  #chdir($YaFsmScxmlParser::gFSMDscOutPath);
  my $FSMName = shift;

  outInterfaceFSMActionHandlerHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "I$FSMName" . "ActionHandler.h"); #the fsm action handler interface
  outInterfaceFSMHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "I$FSMName" . ".h"); # fsm interface for triggering fsm
  outInterfaceFSMStateHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "I$FSMName" . "State.h"); # fsm interface for states
  my @genTransitions;
  outFSMStates($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "$FSMName" . "StateImpl",\@genTransitions); # fsm header file
  #print Dumper(@genTransitions) ;

  outFSMHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "$FSMName" . ".h",\@genTransitions); # fsm header file
  outFSMStateBaseHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "$FSMName" . "StateBase.h"); # fsm base class for state implementation
}

sub outInterfaceFSMActionHandlerHeader
{
  my $FSMName = shift;
  my $outFilePath = shift;
  #my @actions = shift;
  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";


  print $fh "#ifndef I" .uc($FSMName). "ACTIONHANDLER_H\n";
  print $fh "#define I" .uc($FSMName). "ACTIONHANDLER_H\n";
  print $fh "//includes by xml definition\n";
  foreach my $file ( @YaFsmScxmlParser::gFSMIncludes )
  {
    print $fh "#include \"$file\"\n";
  }

  print $fh "#endif\n";

  close( $fh );
}


sub outInterfaceFSMHeader
{
  my $FSMName = shift;
  my $outFilePath = shift;
  #my @actions = shift;
  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";
  print $fh "#ifndef I" . uc($FSMName) . "_H\n";
  print $fh "#define I" . uc($FSMName) . "_H\n";
  print $fh "\n";

#  print $fh "//includes by xml definition\n";
#  foreach my $file ( @YaFsmScxmlParser::gFSMIncludes )
#  {
#    print $fh "#include \"$file\"\n";
#  }

#  print $fh "\n";
#  print $fh "class I" . $FSMName . "\n";
#  print $fh "{\n";
#  print $fh "public:\n";
#  print $fh "  I" . $FSMName . "() {}\n";
#  print $fh "  virtual ~I" . $FSMName . "() {}\n";
#  print $fh "\n";
#  print $fh "  // definition of triggers\n";
#  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTriggers) )
#  {
#    #YaFsm::printDbg("trigger: $key ( $value )");
#    if( !($key =~ m/^timer/) )
#    {
#      print $fh "public:\n";
#    }
#    else
#    {
#      print $fh "protected:\n";
#    }

#    if ( $value )
#    {
#      print $fh "  virtual void $key( $value ) = 0;\n";
#    }
#    else
#    {
#      print $fh "  virtual void $key( void ) = 0;\n";
#    }
#  }

 # print $fh "};\n";
 # print $fh "\n";
  print $fh "#endif\n";
  close( $fh );
}

sub outInterfaceFSMStateHeader
{
  my $FSMName = shift;
  my $outFilePath = shift;
  #my @actions = shift;
  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";
  print $fh "#ifndef I" . uc($FSMName) . "STATE_H\n";
  print $fh "#define I" . uc($FSMName) . "STATE_H\n";

  foreach my $file ( @YaFsmScxmlParser::gFSMIncludes )
  {
    print $fh "#include \"$file\"\n";
  }

  if( %YaFsmScxmlParser::gFSMDataModel )
  {
    print $fh ("#include \"" . $YaFsmScxmlParser::gFSMDataModel{headerfile} . "\"\n");
  }



  print $fh "\n";
  print $fh "class " . $FSMName . ";\n";
  print $fh "\n";
  print $fh "class I" . $FSMName . "State\n";
  print $fh "{\n";
  print $fh "public:\n";
  print $fh "  I" . $FSMName . "State() {}\n";
  print $fh "  virtual ~I" . $FSMName . "State() {}\n";
  print $fh "\n";
  print $fh "  virtual void enter(" . $FSMName . "&) = 0;\n";
  print $fh "  virtual void exit(" . $FSMName . "&) = 0;\n";
  print $fh "\n";
  print $fh "  // definiton of triggers\n";
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTriggers) )
  {
   # YaFsm::printDbg("trigger: $key ( $value )");
    print $fh "  virtual void send_$key( " . $FSMName . "&, const $key" . "& ) = 0;\n";
  }


  print $fh "};\n";
  print $fh "\n";
  print $fh "#endif\n";
  close( $fh );
}

sub printTriggerImpl
{
  my $FSMName = shift;
  my $fh = shift;
  my $key = shift;
  my $value = shift;

  print $fh "{\n";
  print $fh "  if( 0 != mpoCurrentState )\n";
  print $fh "  {\n";
  print $fh "    if( isInitialised() )\n";
  print $fh "    {\n";
  print $fh "      if( !isLocked() )\n";
  print $fh "      {\n";
  print $fh "        setLocked( true );\n";
  print $fh "        mpoCurrentState->send_$key( self(), _event );\n";
  print $fh "        setLocked( false );\n";
  print $fh "      }\n";
  print $fh "      else\n";
  print $fh "      {\n";
  print $fh "        std::cerr << ( \"forbidden call to trigger ". $key . " from action\" ) << std::endl;\n";
  print $fh "      }\n";
  print $fh "    }\n";
  print $fh "    else\n";
  print $fh "    {\n";
  print $fh "      std::cerr << ( \"call to trigger ". $key . " before initFSM()\" ) << std::endl;\n";
  print $fh "    }\n";

  print $fh "  }\n";
  print $fh "}\n";
}

sub outFSMHeader
{
  my $FSMName = shift;
  my $outFilePath = shift;
  my $genTransitions = shift; # array reference
  #my @actions = shift;
  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";
  print $fh "#ifndef _" . uc($FSMName) . "_H\n";
  print $fh "#define _" . uc($FSMName) . "_H\n";

  print $fh "#include \"I" . $FSMName . ".h\"\n";
  print $fh "#include \"" . $FSMName . "StateImpl.h\"\n";

  if( %YaFsmScxmlParser::gFSMDataModel )
  {
    print $fh ("#include \"" . $YaFsmScxmlParser::gFSMDataModel{headerfile} . "\"\n");
  }

  foreach my $member ( @YaFsmScxmlParser::gFSMMembers )
  {
    if ($member->{src})
    {
      print $fh ("#include \"" . $member->{src} . "\"\n");

    }
  }

  print $fh "#include \"IFSMTimerCB.h\"\n";
  print $fh "#include \"IFSMTimer.h\"\n";
  print $fh "#include \"FSMTimer.h\"\n";
  print $fh "#include \"IFSMEventCB.h\"\n";
  print $fh "#include \"IFSMEvent.h\"\n";
  print $fh "#include \"FSMEvent.h\"\n";

  print $fh "\n";
  print $fh "\n";


  print $fh "#include <string>\n";
  print $fh "#include <map>\n";
  print $fh "#include <assert.h>\n";

  print $fh "\n// forward declarations\n";
  print $fh "class " . $FSMName . "StateBase;\n";

  # print $fh "#include \"ProUnit_" . $FSMName . "_.h\"\n";
#  print $fh "\nclass " . $FSMName . ": public I" . $FSMName . "\n";
  print $fh "\nclass " . $FSMName . "\n";
  print $fh " : public IFSMTimerCB\n";
  print $fh " , public IFSMTimer\n";
  print $fh " , public IFSMEventCB\n";
  print $fh " , public IFSMEvent\n";
  print $fh "{\n";
  print $fh "  friend class " . $FSMName . "StateBase;\n";

  foreach my $state (@YaFsmScxmlParser::gFSMStates)
  {
    print $fh '  friend class N' . $FSMName . '::State' . $state . ";\n";
  }

  print $fh "\n";
  print $fh "public:\n";
  print $fh "  " . $FSMName . "()\n";
  print $fh "  : mpoCurrentState( 0 )\n";
  if( %YaFsmScxmlParser::gFSMDataModel )
  {
    print $fh ("  , mDataModel()\n");
  }
  print $fh "  , mbLockTrigger( false )\n";
  print $fh "  , mbInit( false )\n";
  print $fh "  , mFSMTimer( self() )\n";
  print $fh "  , mFSMEvent( self() )\n";
  foreach my $member ( @YaFsmScxmlParser::gFSMMembers )
  {
    if ($member->{src})
    {
      print $fh "  , $member->{id}()\n";
    }
    elsif ( $member->{expr} )
    {
      print $fh "  , $member->{id}($member->{expr})\n";
    }
  }
  print $fh "  {\n";
  my $fsm = \%YaFsmScxmlParser::gFSM;

  foreach my $state (@YaFsmScxmlParser::gFSMStates)
  {
    print $fh '    moStateMap["' . $state .'"] = &moState' . $state . ";\n";
    print $fh '    moStateCoverageMap["' . $state .'"] = 0' .";\n";
  }

  foreach my $trans (@{$genTransitions})
  {
    print $fh '    moTransCoverageMap["' . $trans .'"] = 0' .";\n";
  }
  #while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTimers) )
  foreach my $key (keys(%YaFsmScxmlParser::gFSMTimers))
  {
     YaFsm::printDbg("$key => $YaFsmScxmlParser::gFSMTimers{$key}{ms}");
     YaFsm::printDbg("$key => $YaFsmScxmlParser::gFSMTimers{$key}{cnt}");
     print $fh '    setTimerID(' . uc("TIMER_".$key) .", " . $YaFsmScxmlParser::gFSMTimers{$key}{ms} .", " . $YaFsmScxmlParser::gFSMTimers{$key}{cnt}. ");\n";
  }

  foreach my $key (keys(%YaFsmScxmlParser::gFSMEvents))
 # while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
  {
    print $fh '    registerEventID(' . "EVENT_" . $key . ");\n";
  }

  YaFsm::printDbg("get default enter state name for mpoCurrentState");
  my $enterStateName = $YaFsmScxmlParser::gFSM->{initial};
  if( defined $enterStateName )
  {
    print $fh "    mpoCurrentState = &moState". $enterStateName . ";\n";
  }
  else
  {
    YaFsm::printFatal("required enter state missing");
  }


  print $fh "  }\n";
  print $fh "  virtual ~" . $FSMName . "() {}\n";
  print $fh "\n";

  print $fh "  void initFSM( void );\n";
  print $fh "  virtual void setTimerID( int iTimerId, int iTimeOutMs, int iRepeatCnt );\n";
  print $fh "  virtual void sendEventID( int iEventId );\n";

  if( %YaFsmScxmlParser::gFSMDataModel )
  {
    print $fh "  $YaFsmScxmlParser::gFSMDataModel{classname}& model() { return mDataModel; }\n";
  }


  print $fh "  // definiton of triggers\n";
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTriggers) )
  {
   # YaFsm::printDbg("trigger: $key ( $value )");
    if( !($key =~ m/^timer/) )
    {
      print $fh "public:\n";
    }
    else
    {
      print $fh "protected:\n";
    }

    print $fh "  void sendEvent( const $key" . "& );\n";
  }

  print $fh "\npublic:\n";

  print $fh "// for getting statistics information\n";
  print $fh "  void dumpCoverage( void ) const;\n";


  print $fh "\n  protected:\n";

  print $fh "  void setStateByName( const std::string& name );\n";
  print $fh "  void setTransByName( const std::string& name );\n";
  print $fh "  void enterCurrentState();\n";
  print $fh "  void exitState( const std::string& name );\n";
  print $fh "  virtual void startTimerID( int iTimerId );\n";
  print $fh "  virtual void stopTimerID( int iTimerId );\n";
  print $fh "  virtual void processTimerEventID( int iTimerId );\n";
  print $fh "  virtual void processEventID( int iEventId );\n";

  print $fh "\n";
  print $fh "//todo make this private and allow test makros to access this\n";

  print $fh "#ifdef TESTFSM\n";
  print $fh " public: const std::string& getStateName() const;\n";
  print $fh "#else\n";
  print $fh "  const std::string& getStateName() const;\n";
  print $fh "#endif\n";

  print $fh "  enum " . uc($FSMName) . "TIMER\n";
  print $fh "  {\n";
  my $enumIdx = 0;
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTimers) )
  {
#    if ( $value )
    {
      if(0 == $enumIdx)
      {
        print $fh "    " . uc("TIMER_".$key)."=1,\n";
      }
      else
      {
        print $fh "    " . uc("TIMER_".$key).",\n";
      }
      $enumIdx++;
    }
  }
  print $fh "  };\n";

  # define events enumeration
  print $fh "  public:\n";
  print $fh "  enum " . uc($FSMName) . "EVENT\n";
  print $fh "  {\n";
  $enumIdx = 0;
  foreach my $key (keys(%YaFsmScxmlParser::gFSMEvents))
 # while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
  {
    #die("died");
    if(0 == $enumIdx)
    {
      print $fh "    " . "EVENT_".$key ."=1,\n";
    }
    else
    {
      print $fh "    " . "EVENT_".$key.",\n";
    }
    $enumIdx++;
  }
  print $fh "  };\n";


  print $fh "  private:\n";
  print $fh "  //" . $FSMName . "StateBase& ( const std::string& name );\n";
  print $fh "  " . $FSMName . "& self();\n";
  print $fh "  bool isLocked( void );\n";
  print $fh "  void setLocked( bool );\n";
  print $fh "  bool isInitialised( void );\n";
  print $fh "  void registerEventID( int );\n";

  print $fh "  " . $FSMName . "StateBase* mpoCurrentState;\n";
  if( %YaFsmScxmlParser::gFSMDataModel )
  {
    print $fh ("  $YaFsmScxmlParser::gFSMDataModel{classname} mDataModel;\n");
  }

  print $fh "\n";
  print $fh "  // definition of all states as members\n";
  foreach my $state (@YaFsmScxmlParser::gFSMStates)
  {
    print $fh "  N" . $FSMName. "::State". $state . " moState" . $state . ";\n";
  }
  print $fh "  std::map<std::string, " . $FSMName ."StateBase*> moStateMap;\n";
  print $fh "  std::map<std::string, int> moStateCoverageMap;\n";
  print $fh "  std::map<std::string, int> moTransCoverageMap;\n";
  print $fh "  bool mbLockTrigger;\n";
  print $fh "  bool mbInit;\n";
  print $fh "  FSMTimer mFSMTimer;\n";
  print $fh "  FSMEvent mFSMEvent;\n";
  foreach my $member ( @YaFsmScxmlParser::gFSMMembers )
  {
    if ($member->{src})
    {
        print $fh "  member->{classname}  member->{id};\n";

    }
  }
  print $fh "\n";
  print $fh "};\n";
  print $fh "\n";
  print $fh "inline " . $FSMName . "& " . $FSMName . "::self()\n";
  print $fh "{\n";
  print $fh "  return (*this);\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::setStateByName( const std::string& name)\n";
  print $fh "{\n";
  print $fh "  // set states by names\n";
  print $fh "  if (moStateMap.end() != moStateMap.find(name) )\n";
  print $fh "  {\n";
  print $fh "    mpoCurrentState =  moStateMap[name];\n";
  print $fh "    moStateCoverageMap[name] = moStateCoverageMap[name] + 1;\n";
  print $fh "  }\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::setTransByName( const std::string& name)\n";
  print $fh "{\n";
  print $fh "  // set transitions by names\n";
  print $fh "  if (moTransCoverageMap.end() != moTransCoverageMap.find(name) )\n";
  print $fh "  {\n";
  print $fh "    moTransCoverageMap[name] = moTransCoverageMap[name] + 1;\n";
  print $fh "  }\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline const std::string& " . $FSMName . "::getStateName() const\n";
  print $fh "{\n";
  print $fh "  return mpoCurrentState->getStateName();\n";
  print $fh "\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::enterCurrentState()\n";
  print $fh "{\n";
  print $fh "  mpoCurrentState->enter(self());\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::exitState( const std::string& name )\n";
  print $fh "{\n";
  print $fh "  moStateMap[name]->exit(self());\n";
  print $fh "}\n\n";
  print $fh "inline bool " . $FSMName . "::isLocked( void )\n";
  print $fh "{\n";
  print $fh "  return mbLockTrigger;\n";
  print $fh "}\n\n";
  print $fh "inline void " . $FSMName . "::setLocked( bool bLocked )\n";
  print $fh "{\n";
  print $fh "  mbLockTrigger = bLocked;\n";
  print $fh "}\n";
  print $fh "inline bool " . $FSMName . "::isInitialised( void )\n";
  print $fh "{\n";
  print $fh "  return mbInit;\n";
  print $fh "}\n\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::initFSM( void )\n";
  print $fh "{\n";
  print $fh "  mbInit = true;\n";
  print $fh "  enterCurrentState();\n";
  print $fh "}\n\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::setTimerID( int iTimerID, int iTimeoutMs, int iRepeatCnt )\n";
  print $fh "{\n";
  print $fh "  if( !isInitialised() )\n";
  print $fh "  {\n";
  print $fh "    mFSMTimer.setTimerID( iTimerID, iTimeoutMs, iRepeatCnt );\n";
  print $fh "  }\n";
  print $fh "  else\n";
  print $fh "  {\n";
  print $fh "    std::cerr << ( \"forbidden call to setTimerID after call to initFSM()\" ) << std::endl;\n";
  print $fh "  }\n";
  print $fh "}\n";
  print $fh "inline void " . $FSMName . "::stopTimerID( int iTimerID )\n";
  print $fh "{\n";
  #print $fh "  std::cout <<\"stopTimerID\" << std::endl;\n";
  print $fh "  mFSMTimer.stopTimerID( iTimerID );\n";
  print $fh "}\n";
  print $fh "inline void " . $FSMName . "::startTimerID( int iTimerID )\n";
  print $fh "{\n";
  #print $fh "  std::cout <<\"startTimerID\" << std::endl;\n";
  print $fh "  mFSMTimer.startTimerID( iTimerID );\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::processTimerEventID( int iTimerID )\n";
  print $fh "{\n";
# definition of all timers as enumeration
  print $fh "  (void) iTimerID;\n\n";

  if(%YaFsmScxmlParser::gFSMTimers)
  {
    my $numTimers = keys(%YaFsmScxmlParser::gFSMTimers);
    if($numTimers)
    {
      print $fh "\n";
      print $fh "  switch(iTimerID)\n";
      print $fh "  {\n";

      use Data::Dumper;
      #while( my( $key, %value ) = each( %YaFsmScxmlParser::gFSMTimers) )
      foreach my $key (keys(%YaFsmScxmlParser::gFSMTimers))
      {
        #if ( $value )
        {
          print $fh "  case " . uc("TIMER_".$key).":\n";
          print $fh "    timer" . $key ."();\n";
          print $fh "  break;\n";
        }
      }

      print $fh "  default:\n";
      print $fh "    std::cerr << \"TimerID \" << iTimerID << \" not handled\" << std::endl;\n";
      print $fh "  break;\n";
      print $fh "  }\n";
    }
  }

  print $fh "}\n";
  print $fh "\n";

  print $fh "inline void " . $FSMName . "::registerEventID( int iEventID )\n";
  print $fh "{\n";
  print $fh "  mFSMEvent.registerEventID( iEventID );\n";
  print $fh "}\n";

  print $fh "inline void " . $FSMName . "::sendEventID( int iEventID )\n";
  print $fh "{\n";
  print $fh "  mFSMEvent.sendEventID( iEventID );\n";
  print $fh "}\n";
  print $fh "\n";

  print $fh "inline void " . $FSMName . "::processEventID( int iEventID )\n";
  print $fh "{\n";
  print $fh "  (void) iEventID;\n";

  if(%YaFsmScxmlParser::gFSMEvents)
  {
    # definition of all timers as enumeration
    print $fh "\n";
    print $fh "  switch(iEventID)\n";
    print $fh "  {\n";
    foreach my $key (keys(%YaFsmScxmlParser::gFSMEvents))
   # while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
    {
      YaFsm::printDbg("events: $key ");
      print $fh "  case " . "EVENT_".$key.":\n";
      print $fh "    $key event;\n";
      print $fh "    sendEvent" . "(event);\n";
      print $fh "  break;\n";
    }

    print $fh "  default:\n";
    print $fh "  break;\n";
    print $fh "  }\n";
  }

  print $fh "}\n";
  print $fh "\n";

  print $fh "// declaration of all triggers\n";
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTriggers) )
  {

    print $fh "inline void " . $FSMName . "::sendEvent( const $key" . "& _event)\n";
    printTriggerImpl($FSMName, $fh, $key, $value);
    print $fh "\n";

  }
  print $fh "\n";
  print $fh "inline void " . $FSMName . "::dumpCoverage( void ) const\n";
  print $fh "{\n";
  print $fh "  int iStatesCovered = 0;\n";
  print $fh "  int iTransCovered = 0;\n";
  print $fh "  std::cout << std::endl << \"Dumping coverage information:\" << std::endl;\n";
  print $fh "  std::map<std::string, int>::const_iterator it;\n";
  print $fh "  for(it = moStateCoverageMap.begin(); it != moStateCoverageMap.end(); ++it)\n";
  print $fh "  {\n";
  print $fh "    std::cout << \"  state\" << it->first << \" covered \"<< it->second << \" times\" << std::endl;\n";
  print $fh "    if(0 < it->second)\n";
  print $fh "    {\n";
  print $fh "      iStatesCovered++;\n";
  print $fh "    }\n";
  print $fh "  }\n";

  print $fh "  for(it = moTransCoverageMap.begin(); it != moTransCoverageMap.end(); ++it)\n";
  print $fh "  {\n";
  print $fh "    std::cout << \"  transition \" << it->first << \" covered \"<< it->second << \" times\" << std::endl;\n";
  print $fh "    if(0 < it->second)\n";
  print $fh "    {\n";
  print $fh "      iTransCovered++;\n";
  print $fh "    }\n";
  print $fh "  }\n";

  print $fh "  std::cout << std::endl << \"  total coverage:\" << std::endl;\n";
  print $fh "  std::cout << \"  States covered: \" << iStatesCovered << \" out of \"<< moStateCoverageMap.size() << \", \";\n";
  print $fh "  std::cout << (static_cast<size_t>(iStatesCovered)*1.0) / (1.0*moStateCoverageMap.size()) * 100.0 << \" percent\" << std::endl;\n";
  print $fh "  std::cout << \"  Transitions covered: \" << iTransCovered << \" out of \"<< moTransCoverageMap.size() << \", \";\n";
  print $fh "  std::cout << (static_cast<size_t>(iTransCovered)*1.0) / (1.0*moTransCoverageMap.size()) * 100.0 << \" percent\" << std::endl;\n";

  print $fh "  \n";
  print $fh "  \n";
  print $fh "  \n";
  print $fh "}\n";

  print $fh "\n";


  print $fh "#endif\n";
  close( $fh );
}


sub outFSMStateBaseHeader
{
  my $FSMName = shift;
  my $outFilePath = shift;
  #my @actions = shift;
  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";
  print $fh "#ifndef _" . uc($FSMName) . "StateBase_H\n";
  print $fh "#define _" . uc($FSMName) . "StateBase_H\n";
  print $fh "\n";
  print $fh "#include \"I" . $FSMName . "State.h\"\n";

  print $fh "//includes by xml definition\n";
  foreach my $file ( @YaFsmScxmlParser::gFSMIncludes )
  {
    print $fh "#include \"$file\"\n";
  }

  print $fh "#include <string>\n";
  print $fh "#include <iostream>\n";
  print $fh "#include <sstream>\n";


  print $fh "\n";
  print $fh "\n";

  print $fh "class " . $FSMName . "StateBase: public I" . $FSMName . "State\n";

  print $fh "{\n";
  print $fh "  friend class " . $FSMName . ";\n";
  print $fh "\n";
  print $fh "public:\n";
  print $fh "  " . $FSMName . "StateBase( )\n";
  print $fh "   : mStateName()\n";
  print $fh "    {\n";
  print $fh "    }\n";


  print $fh "  virtual ~" . $FSMName . "StateBase() {}\n";
  print $fh "\n";
  print $fh "protected:\n";
  print $fh "  const std::string& getStateName() const;\n";
  print $fh "  void setStateName(const std::string&);\n";
  print $fh "\n";
  print $fh "  // definition of all triggers\n";

  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTriggers) )
  {
    #YaFsm::printDbg("trigger: $key ( $value )");
    print $fh "  inline virtual void send_$key( " . $FSMName . "&, const $key". "& _event)\n";
    print $fh "  {\n";
    print $fh "  }\n";
  }

  print $fh "\n";

  print $fh "\n";
  print $fh "protected:\n";
  print $fh "  std::string mStateName;\n";

  print $fh "};\n";
  print $fh "\n";
  print $fh "inline const std::string& " . $FSMName . "StateBase::getStateName() const\n";
  print $fh "{\n";
  print $fh "  return mStateName;\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "StateBase::setStateName(const std::string& str)\n";
  print $fh "{\n";
  print $fh "  mStateName = str;\n";
  print $fh "}\n";
  print $fh "\n";


  print $fh "#endif\n";
  close( $fh );
}


sub outFSMStates
{
  my $FSMName = shift;
  my $outFilePath = shift;
  my $genTransitions = shift; #array reference

  my $outFilePathHeader = $outFilePath .".h";
  my $outFilePathSource = $outFilePath .".cpp";

  open ( my $fhH, ">$outFilePathHeader") or YaFsm::printFatal "cannot open file $outFilePathHeader for writing";
  open ( my $fhS, ">$outFilePathSource") or YaFsm::printFatal "cannot open file $outFilePathSource for writing";
  print $fhH "#ifndef _" . uc($FSMName) . "STATEIMPL_H\n";
  print $fhH "#define _" . uc($FSMName) . "STATEIMPL_H\n";
  print $fhH "\n";
  print $fhH "#include \"" . $FSMName . "StateBase.h\"\n";
  print $fhH "\n";
  print $fhH "// definitions of all States as classes\n";

  if( %YaFsmScxmlParser::gFSMDataModel )
  {
    print $fhH "class $YaFsmScxmlParser::gFSMDataModel{classname};\n";
  }

  print $fhH "namespace N". $FSMName . "\n";
  print $fhH "{\n";


  print $fhS '#include "' . $FSMName . "StateImpl.h\"\n";
  print $fhS '#include "' . $FSMName . ".h\"\n";

  print $fhS "namespace N". $FSMName . "\n";
  print $fhS "{\n";

  parseFSM($fhH,$fhS,$YaFsmScxmlParser::gFSM,"","",\@{$genTransitions});

  print $fhH "}\n";
  print $fhS "}\n";
  # close header
  print $fhH "#endif\n";
  close( $fhH );
  close( $fhS );


}

sub parseFSM
{
  my $fhH = shift;
  my $fhS = shift;

  my $currRef = shift;
  my $parentName = shift;
  my $parentParentName=shift;
  my $genTransitions = shift; #array reference

  YaFsm::printDbg("parseFSM in CppOO");


  #print Dumper($currRef) if $YaFsm::gVerbose;
  foreach my $state (@{$currRef->{state}})
  {
    if(YaFsmScxmlParser::hasSubStates($state))
    {
      genStateImpl($fhH, $fhS, $state,$currRef,$parentName);
      parseFSM($fhH, $fhS, $state, $state->{id}, $parentName,\@{$genTransitions});
    }
    else
    {
      genStateImpl($fhH, $fhS, $state,$currRef,$parentName);
    }
    genStateTransImpl($fhH, $fhS, $state,$parentName,\@{$genTransitions});

  }

  #print   Dumper(@{$genTransitions});
}



sub genStateImpl
{
  my $fhH = shift;
  my $fhS = shift;

  my $state = shift;
  my $currRef = shift;
  my $parentName =shift;

  YaFsm::printDbg("codegen: state $state->{id}");
  YaFsm::printDbg("codegen: parent $parentName") if defined $parentName;

  if(defined $parentName && length($parentName))
  {
    print $fhH "class State".$state->{id}.": public State" . $parentName ."\n";
  }
  else
  {
    print $fhH "class State".$state->{id}.": public " . $YaFsmScxmlParser::gFSMName . "StateBase\n";
  }
  print $fhH "{\n";
  print $fhH "public:\n";
  print $fhH "  State" . $state->{id} . "();\n";
  print $fhH "  virtual ~State" . $state->{id} . "();\n";
  # declare all triggers handled by this state
  print $fhH "protected:\n";
  my %processedTriggers;
  foreach my $trans (@{$state->{transition}})
  {
    if($trans->{event} && !(exists($processedTriggers{$trans->{event}})))
    {
      if( $trans->{source} eq $state->{id} )
      {
        print $fhH "  virtual void send_$trans->{event}( ". $YaFsmScxmlParser::gFSMName . "&, const $trans->{event}" . "& _event );\n";
        $processedTriggers{$trans->{event}}="";
      }
    }
  }
  print $fhH "private:\n";
  print $fhH "  void enter( " . $YaFsmScxmlParser::gFSMName . "& );\n";
  print $fhH "  void exit( " . $YaFsmScxmlParser::gFSMName . "& );\n";

  # implement actions
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMActions) )
  {
    foreach my $action (@{$value})
    {
      #print Dumper($action);
      if($key eq $state->{id} )
      {
        #YaFsm::printDbg("trigger: $key ( $value )");
        if( $action->{script})
        {
          if( %YaFsmScxmlParser::gFSMDataModel )
          {
            if( $action->{event} )
            {
              print $fhH "  void $action->{name}( " .  $YaFsmScxmlParser::gFSMDataModel{classname} . "& model, const $action->{event} ". "& _event );\n";
            }
            else
            {
              print $fhH "  void $action->{name}( " .  $YaFsmScxmlParser::gFSMDataModel{classname} . "& model );\n";
            }

          }
          else
          {
            print $fhH "  void $action->{name}();\n";
          }
        }
      }
    }
  }


  # close state header class
  print $fhH "};\n\n";

  print $fhS "State".$state->{id}."::State".$state->{id}."()\n";
  print $fhS "{\n";
  print $fhS "  setStateName( \"State". $state->{id} ."\" );\n";
  print $fhS "}\n\n";

  print $fhS "State".$state->{id}."::~State".$state->{id}."()\n";
  print $fhS "{\n";
  print $fhS "}\n\n";


  # implement actions
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMActions) )
  {
    foreach my $action (@{$value})
    {
#      print Dumper($action);
      if($key eq $state->{id} )
      {
        #YaFsm::printDbg("trigger: $key ( $value )");
        if( $action->{script})
        {
          if( %YaFsmScxmlParser::gFSMDataModel )
          {
            if( $action->{event} )
            {
              print $fhS "void State".$state->{id}."::$action->{name}( " .  $YaFsmScxmlParser::gFSMDataModel{classname} . "& model, const $action->{event} " . "& _event )\n";
            }
            else
            {
              print $fhS "void State".$state->{id}."::$action->{name}( " .  $YaFsmScxmlParser::gFSMDataModel{classname} . "& model )\n";
            }
          }
          else
          {
            print $fhS "void State".$state->{id}."::$action->{name}()\n";
          }
          print $fhS "{\n";
          my @codeList = $action->{script};
          foreach (@codeList)
          {
            print $fhS "  $_\n";
          }
          print $fhS "}\n\n";

        }
      }
    }
  }


#      if(YaFsmScxmlParser::hasStateActions($state))
  {
    my $enterStateName = YaFsmScxmlParser::getEnterStateName($state);
    if(( YaFsmScxmlParser::hasStateEnterActions($state) ) || (defined $enterStateName) )
    {
      print $fhS "void State".$state->{id}."::enter( " . $YaFsmScxmlParser::gFSMName . "& fsmImpl )\n";
    }
    else
    {
      print $fhS "void State".$state->{id}."::enter( " . $YaFsmScxmlParser::gFSMName . "& /*fsmImpl*/ )\n";
    }
    print $fhS "{\n";

    if(YaFsmScxmlParser::hasStateEnterActions($state))
    {
      if(defined $state->{tstopenter})
      {
        my @str = split(/;/,$state->{tstopenter});
        foreach(@str)
        {
          print $fhS "  fsmImpl.stopTimerID( " . $YaFsmScxmlParser::gFSMName . "::" . uc("TIMER_" . $_) ." );\n";
        }
      }

      if(defined $state->{tstartenter})
      {
        my @str = split(/;/,$state->{tstartenter});
        foreach(@str)
        {
          print $fhS "  fsmImpl.startTimerID( " . $YaFsmScxmlParser::gFSMName . "::" . uc("TIMER_" . $_) ." );\n";
        }
      }

      if(%YaFsmScxmlParser::gFSMDataModel && defined $state->{onentry}{script})
      {
        print $fhS ( "  " . $state->{id} ."_onEntry(fsmImpl.model());\n" );
      }

      if(defined $state->{onentry}{raise})
      {
        foreach(@{$state->{onentry}{raise}})
        {
          print $fhS "  fsmImpl.sendEventID(" . $YaFsmScxmlParser::gFSMName .'::EVENT_'.$_->{event}.");\n";
        }
      }

    }

    if( defined $enterStateName )
    {
      #if substate is entered no exit of parent state is intended
      print $fhS "  fsmImpl.setStateByName( \"$enterStateName\" );\n";
      print $fhS "  fsmImpl.enterCurrentState();\n";
    }
    print $fhS "}\n\n";


    if(YaFsmScxmlParser::hasStateExitActions($state))
    {
      print $fhS "void State".$state->{id}."::exit( " . $YaFsmScxmlParser::gFSMName . "& fsmImpl )\n";
      print $fhS "{\n";

      if(defined $state->{tstopexit})
      {
        my @str = split(/;/,$state->{tstopexit});
        foreach(@str)
        {
          print $fhS "  fsmImpl.stopTimerID( " . $YaFsmScxmlParser::gFSMName . "::" . uc("TIMER_" . $_) ." );\n";
        }
      }

      if(defined $state->{tstartexit})
      {
        my @str = split(/;/,$state->{tstartexit});
        foreach(@str)
        {
          print $fhS "  fsmImpl.startTimerID( " . $YaFsmScxmlParser::gFSMName . "::" . uc("TIMER_" . $_) ." );\n";
        }
      }

      if(%YaFsmScxmlParser::gFSMDataModel && defined $state->{onexit}{script})
      {
        print $fhS ( "  " . $state->{id} . "_onExit(fsmImpl.model());\n" );
      }

      if(defined $state->{onexit}{raise})
      {
        foreach(@{$state->{onexit}{raise}})
        {
          print $fhS "  fsmImpl.sendEventID(" . $YaFsmScxmlParser::gFSMName .'::EVENT_' . $_->{event} . ");\n";
        }
      }

    }
    else
    {
      print $fhS "void State".$state->{id}."::exit( " . $YaFsmScxmlParser::gFSMName . "& /*fsmImpl*/ )\n";
      print $fhS "{\n";

    }
    print $fhS "}\n\n";

    #print $fh "}>\n";

  }
}

sub genStateTransImpl
{
  my $fhH = shift;
  my $fhS = shift;
  my $currRef = shift;
  my $parentName =shift;
  my $genTransitions = shift; #array reference


  my %processedTriggers;
  my $idx = 0;
  my @transArray;
  @transArray = @{$currRef->{transition}} if $currRef->{transition};

  # generate all transitions for the states
  foreach my $trans (@transArray)
  {
    my $criteria = $trans->{source} . '_' . $trans->{event};
    # add methods for each trigger.
    # be carefull that a trigger is not implemented twice, depending on conditions
    # so consider trigger name and begin of a trigger as condition if already implemented

    if($trans->{event} && !(exists($processedTriggers{$criteria})))
    {
      $processedTriggers{$criteria} = "";
      my $transCoverageName;
      $transCoverageName = $trans->{source} .'_' . $trans->{event};
      print $fhS "void State$trans->{source}::send_$trans->{event}( " . $YaFsmScxmlParser::gFSMName . "& fsmImpl, const $trans->{event}" . "& _event)\n";
      print $fhS "{\n";

      push(@{$genTransitions},$transCoverageName);

      print $fhS "  (void) fsmImpl;\n";

      for(my $nextIdx=$idx; $nextIdx < @transArray; $nextIdx++)
      {
        if($transArray[$nextIdx]->{event} eq $trans->{event})
        {
          if($transArray[$nextIdx]->{source} eq $trans->{source})
          {
            if($transArray[$nextIdx]->{cond})
            {
              print $fhS "  if ($transArray[$nextIdx]->{cond})\n"; #todo implement conditions
            }
            print $fhS "  {\n";
            print $fhS "    fsmImpl.setTransByName(\"$transCoverageName\");\n";
            # current could be difficult to determine. if we made a fallthrough into next hierarchy level
            # exit state by name
            if( $transArray[$nextIdx]->{source} ne $transArray[$nextIdx]->{target} )
            {
              print $fhS "    fsmImpl.exitState(\"" . $transArray[$nextIdx]->{source} ."\");\n" ;
            }

            if(YaFsmScxmlParser::hasTransitionActions($transArray[$nextIdx]))
            {
              print $fhS ( "    transition_" . $transArray[$nextIdx]->{source} . "_" . $transArray[$nextIdx]->{event} . "_$idx(fsmImpl.model(), _event);\n" );
            }
            if(YaFsmScxmlParser::hasTransitionEvents($transArray[$nextIdx]))
            {
              foreach(@{$transArray[$nextIdx]->{raise}})
              {
                print $fhS "    fsmImpl.sendEventID(" . $YaFsmScxmlParser::gFSMName .'::EVENT_'. $_->{event} .");\n";
              }
              #  print $fh "<TR><TD>^$trans->{event}</TD></TR>;\n";
            }

            if( $transArray[$nextIdx]->{source} ne $transArray[$nextIdx]->{target} )
            {
              print $fhS '    fsmImpl.setStateByName("' . $transArray[$nextIdx]->{target} . "\");\n";
              print $fhS "    fsmImpl.enterCurrentState();\n";
            }

            if($transArray[$nextIdx]->{cond} && (defined $parentName) && (0 < length($parentName)))
            {
              print $fhS "  }\n";
              print $fhS "  else // condition is not matched, we should now try if condition is matched by parent\n";
              print $fhS "  {\n";

              print $fhS "    State$parentName" . "::send_$trans->{event}( fsmImpl, $trans->{event} );\n";
            }

            print $fhS "  }\n\n";
          }
        }
      }
      print $fhS "}\n\n";
    }
    $idx++;
  }
}

sub getParamsArray
{
  my $strParams = shift;
  my  @params = split(/,/,$strParams);

  return @params;
}

sub getParaTypeName
{
  my $paraStr = shift;

  $paraStr =~  s/^\s+//;
  $paraStr =~ s/\s+$//;
  my $type;
  my $name;

  # type can contain more than one part, e.g const <type>&
  my @list = split(/ /, $paraStr);
  if(0 == $#list)
  {
    YaFsm::printFatal("parameter $_ must contain at least type and parameter name");
  }
  $name = pop(@list);
  $type = join(' ',@list);
  return ($type,$name);

}

END { }       # module clean-up code here (global destructor)

1;

__END__

=head1 NAME

ModuleName - short discription of your program

=head1 SYNOPSIS

 how to us your module

=head1 DESCRIPTION

 long description of your module

=head1 SEE ALSO

 need to know things before somebody uses your program

=head1 AUTHOR

 Joggl

=cut
