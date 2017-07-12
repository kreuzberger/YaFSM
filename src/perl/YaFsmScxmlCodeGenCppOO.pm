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



sub genSendEventImpl
{
  my $fhS = shift;
  my $sendId = shift;
  my $send = shift;

  my $eventPara = YaFsmScxmlParser::getEventPara($send);
  print $fhS "    $send->{event} data" . $send->{event} . ";\n";
  foreach my $para ( @{$eventPara->{param}} )
  {
    print $fhS "    data" . $send->{event} . ".$para->{name} = $para->{expr};\n" if( $para->{expr} );
  }

  my $strID = "";

  if($send->{id})
  {
    YaFsm::printWarn("use of generated ids for send event not allowed!\n!");
  }
  elsif($send->{idlocation})
  {
    YaFsm::printWarn("use of idlocation for send event not allowed!\n!");
  }
  print $fhS "    " . $strID . "fsmImpl.sendEvent( \"$sendId\", data" . $send->{event} . ", $eventPara->{delay});\n";

}


sub genCancelEventImpl
{
  my $fhS = shift;
  my $cancel = shift;

  print $fhS "  fsmImpl.cancelEvent(\"" . $cancel->{sendid} . "\");\n";
}

sub writeCodeFiles
{
  my $FSMName = shift;

  outInterfaceFSMStateHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "I$FSMName" . "State.h"); # fsm interface for states
  my @genTransitions;
  outFSMStates($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "$FSMName" . "StateImpl",\@genTransitions); # fsm header file
  #print Dumper(@genTransitions) ;

  outFSMHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "$FSMName" . ".h",\@genTransitions); # fsm header file
  outFSMStateBaseHeader($FSMName, $YaFsmScxmlParser::gFSMCodeOutPath . '/' . "$FSMName" . "StateBase.h"); # fsm base class for state implementation
}




sub outInterfaceFSMStateHeader
{
  my $FSMName = shift;
  my $outFilePath = shift;
  #my @actions = shift;
  open ( my $fh, ">$outFilePath") or YaFsm::printFatal "cannot open file $outFilePath for writing";
  print $fh "#ifndef I" . uc($FSMName) . "STATE_H\n";
  print $fh "#define I" . uc($FSMName) . "STATE_H\n";

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

  print $fh "#include \"IScxmlFSMEventCB.h\"\n";
  print $fh "#include \"IScxmlFSMEvent.h\"\n";
  print $fh "#include \"ScxmlFSMEvent.h\"\n";

  print $fh "\n";
  print $fh "\n";


  print $fh "#include <string>\n";
  print $fh "#include <map>\n";
  print $fh "#include <vector>\n";
  print $fh "#include <assert.h>\n";

  print $fh "\n// forward declarations\n";
  print $fh "class " . $FSMName . "StateBase;\n";

  # print $fh "#include \"ProUnit_" . $FSMName . "_.h\"\n";
#  print $fh "\nclass " . $FSMName . "\n";
  print $fh "\nclass " . $FSMName . "\n";
  print $fh " : public IScxmlFSMEventCB\n";
#  print $fh " , public IScxmlFSMEvent\n";
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
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
  {
    print $fh '    mFSMEvent.setEventID(' . "EVENT_".$key .");\n";
  }


  YaFsm::printDbg("get default enter state name for mpoCurrentState");
  my $enterStateName = $YaFsmScxmlParser::gFSM->{initial};
  if( defined $enterStateName )
  {
    print $fh "    setStateByName(\"$enterStateName\");\n";
  }
  else
  {
    YaFsm::printFatal("required enter state missing");
  }


  print $fh "  }\n";
  print $fh "  virtual ~" . $FSMName . "() {}\n";
  print $fh "\n";

  print $fh "  void initFSM( void );\n";
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
  {
    print $fh "  virtual int sendEvent( const std::string& sendId, const " . $key . "& data, int iDelayMs);\n";
  }
  print $fh "  virtual void cancelEvent( const std::string& sendId );\n";

  if( %YaFsmScxmlParser::gFSMDataModel )
  {
    print $fh "  $YaFsmScxmlParser::gFSMDataModel{classname}& model() { return mDataModel; }\n";
  }


  print $fh "  // definiton of triggers\n";
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMTriggers) )
  {
   # YaFsm::printDbg("trigger: $key ( $value )");
    print $fh "public:\n";
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
  print $fh "  virtual void processTimerEventID( int event, int id );\n";

  print $fh "\n";
  print $fh "//todo make this private and allow test makros to access this\n";

  print $fh "#ifdef TESTFSM\n";
  print $fh " public: const std::string& getStateName() const;\n";
  print $fh "#else\n";
  print $fh "  const std::string& getStateName() const;\n";
  print $fh "#endif\n";

  # define events enumeration
  print $fh "  public:\n";
  print $fh "  enum " . uc($FSMName) . "EVENT\n";
  print $fh "  {\n";
  my $enumIdx = 0;
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
  print $fh "  void exitSubStates(const std::string& name );\n";

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
  print $fh "  ScxmlFSMEvent mFSMEvent;\n";
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
  {
    print $fh "  std::map<int, $key> mParaMap_$key;\n";
  }

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
  print $fh "  exitSubStates(name);\n";
  print $fh "  moStateMap[name]->exit(self());\n";
  print $fh "}\n\n";
  print $fh "inline void " . $FSMName . "::exitSubStates( const std::string& name )\n";
  print $fh "{\n";
  print $fh "  std::vector<std::string> subStates;\n";
  print $fh "  std::string stateName = mpoCurrentState->getStateName();\n";
  print $fh "  while(name != stateName)\n";
  print $fh "  {\n";
  print $fh "    moStateMap[stateName]->exit(self());\n";
  print $fh "    stateName = moStateMap[stateName]->getParentStateName();\n";
  print $fh "  }\n";
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
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
  {
    print $fh "inline int " . $FSMName . "::sendEvent( const std::string& sendId, const " . $key . "& data, int iDelayMs )\n";
    print $fh "{\n";
    print $fh "  int id = mFSMEvent.sendEventID( sendId + \"." . $key . "\", EVENT_". $key." , iDelayMs );\n";
    print $fh "  mParaMap_". $key . "[id] = data;\n";
    print $fh "  return id;\n";
    print $fh "}\n";
  }

  print $fh "inline void " . $FSMName . "::cancelEvent( const std::string& sendId )\n";
  print $fh "{\n";
  print $fh "  std::vector<int> ids = mFSMEvent.cancelEvent( sendId );\n";
  print $fh "  for(auto it = ids.begin(); it != ids.end(); ++it )\n";
  print $fh "  {\n";
  while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
  {
    print $fh "    if( mParaMap_". $key . ".find(*it) != mParaMap_" . $key . ".end()) { mParaMap_". $key . ".erase(*it); }\n";
  }
  print $fh "  }\n";
  print $fh "}\n";


print $fh "inline void " . $FSMName . "::processTimerEventID( int event, int id )\n";
  print $fh "{\n";
  # definition of all events as enumeration

  if(%YaFsmScxmlParser::gFSMEvents)
  {
    # definition of all timers as enumeration
    print $fh "\n";
    print $fh "  switch(event)\n";
    print $fh "  {\n";
    foreach my $key (keys(%YaFsmScxmlParser::gFSMEvents))
   # while( my( $key, $value ) = each( %YaFsmScxmlParser::gFSMEvents) )
    {
      YaFsm::printDbg("events: $key ");
      print $fh "  case " . "EVENT_".$key.":\n";
      print $fh "  {\n";
      print $fh "    $key event = mParaMap_". $key. "[id];\n";
      print $fh "    sendEvent" . "(event);\n";
      print $fh "    mParaMap_$key.erase(id);\n";
      print $fh "  }\n";
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
  print $fh "   , mParentStateName()\n";
  print $fh "    {\n";
  print $fh "    }\n";


  print $fh "  virtual ~" . $FSMName . "StateBase() {}\n";
  print $fh "\n";
  print $fh "protected:\n";
  print $fh "  const std::string& getStateName() const;\n";
  print $fh "  const std::string& getParentStateName() const;\n";
  print $fh "  void setStateName(const std::string&);\n";
  print $fh "  void setParentStateName(const std::string&);\n";
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
  print $fh "  std::string mParentStateName;\n";

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
  print $fh "inline const std::string& " . $FSMName . "StateBase::getParentStateName() const\n";
  print $fh "{\n";
  print $fh "  return mParentStateName;\n";
  print $fh "}\n";
  print $fh "\n";
  print $fh "inline void " . $FSMName . "StateBase::setParentStateName(const std::string& str)\n";
  print $fh "{\n";
  print $fh "  mParentStateName = str;\n";
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
  print $fhS "#include <vector>\n";
  print $fhS "#include <iostream>\n";

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

  my $baseClassName = "";

  YaFsm::printDbg("codegen: state $state->{id}");
  YaFsm::printDbg("codegen: parent $parentName") if defined $parentName;

  if(defined $parentName && length($parentName))
  {
    print $fhH "class State".$state->{id}.": public State" . $parentName ."\n";
    $baseClassName = $parentName;
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
  print $fhS "  setStateName( \"". $state->{id} ."\" );\n";
  print $fhS "  setParentStateName( \"". $baseClassName ."\" );\n" if( 0 < length($baseClassName));
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
      if(%YaFsmScxmlParser::gFSMDataModel && defined $state->{onentry}{script})
      {
        print $fhS ( "  " . $state->{id} ."_onEntry(fsmImpl.model());\n" );
      }

      if(defined $state->{onentry}{raise})
      {
        foreach(@{$state->{onentry}{raise}})
        {
          print $fhS "  fsmImpl.sendEvent(\"$state->{id}\", " . $_->{event}."(), 0);\n";
        }
      }
      if(defined $state->{onentry}{send})
      {
        foreach(@{$state->{onentry}{send}})
        {
          genSendEventImpl($fhS, $state->{id}, $_);
        }
      }
      if(defined $state->{onentry}{cancel})
      {
        foreach(@{$state->{onentry}{cancel}})
        {
          genCancelEventImpl($fhS, $_);
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


      if(%YaFsmScxmlParser::gFSMDataModel && defined $state->{onexit}{script})
      {
        print $fhS ( "  " . $state->{id} . "_onExit(fsmImpl.model());\n" );
      }

      if(defined $state->{onexit}{raise})
      {
        foreach(@{$state->{onexit}{raise}})
        {
          print $fhS "  fsmImpl.sendEvent( \"$state->{id}\", ". $_->{event} . "(), 0);\n";
        }
      }

      if(defined $state->{onexit}{send})
      {
        foreach(@{$state->{onexit}{send}})
        {
          genSendEventImpl($fhS, $state->{id}, $_);
        }
      }
      if(defined $state->{onexit}{cancel})
      {
        foreach(@{$state->{onexit}{cancel}})
        {
          genCancelEventImpl($fhS, $_);
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
                print $fhS "    fsmImpl.sendEvent( \"$transArray[$nextIdx]->{event}\", " . $_->{event} . "(), 0);\n";
              }
              foreach(@{$transArray[$nextIdx]->{send}})
              {
                genSendEventImpl($fhS, $transArray[$nextIdx]->{event}, $_);
              }
              foreach(@{$transArray[$nextIdx]->{cancel}})
              {
                genCancelEventImpl($fhS, $_);
              }
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
