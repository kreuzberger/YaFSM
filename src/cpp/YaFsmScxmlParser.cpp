#include "YaFsmScxmlParser.h"
#include "YaFsm.h"
#include <iostream>
#include <fstream>
#include <algorithm>

YaFsmScxmlParser::YaFsmScxmlParser()
{
}
std::string YaFsmScxmlParser::fileName() const
{
  return mFileName;
}

void YaFsmScxmlParser::setFileName( const std::string& fileName )
{
  mFileName = fileName;
}

std::string YaFsmScxmlParser::codeOutDir() const
{
  return mCodeOutDir;
}

void YaFsmScxmlParser::setCodeOutDir( const std::string& codeOutDir )
{
  mCodeOutDir = codeOutDir;
}

void YaFsmScxmlParser::setNamespace( const std::string& ns )
{
  mNamespace = ns;
}

std::string YaFsmScxmlParser::ns() const
{
  return mNamespace;
}

void YaFsmScxmlParser::init()
{
}

void YaFsmScxmlParser::readFSM()
{

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError    error = doc.LoadFile( mFileName.c_str() );
  if ( tinyxml2::XML_SUCCESS == error )
  {
    size_t idx = mFileName.rfind( ".scxml" );
    if ( idx != std::string::npos )
    {
      std::string name = mFileName.substr( 0, idx );
      idx              = name.rfind( YaFsm::sep );
      if ( idx == std::string::npos )
      {
        idx = name.rfind( "/" );
      }
      if ( idx != std::string::npos )
      {
        name = name.substr( idx + 1, name.length() );
        if ( !name.empty() )
        {
          mDataModel["name"] = name;
        }
        else
        {
          YaFsm::printDbg( std::string( "found name " ) + name );
        }
      }
    }

    if ( mDataModel["name"].empty() )
    {
      YaFsm::printFatal( "cannot determine fsm name from filename. Only files with .scxml extension allowd!" );
    }

    tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
    if ( elem )
    {
      parseDefinitions( elem );
      parseFSM( elem );

      // start writing out code files
      writeFSMStates( elem );
      writeFSMHeader( elem );
      writeInterfaceFSMStateHeader();
      writeFSMStateBaseHeader();
    }
  }
  else
  {
    YaFsm::printFatal( "cannot read xml file " + mFileName );
  }
}

void YaFsmScxmlParser::parseDefinitions( const tinyxml2::XMLElement* elem )
{
  const tinyxml2::XMLAttribute* datamodel = elem->FindAttribute( "datamodel" );
  if ( datamodel )
  {
    std::string str = datamodel->Value();
    if ( !str.empty() )
    {
      std::vector<std::string> elems = YaFsm::split( str, ':' );
      if ( 3 == elems.size() )
      {
        if ( "cplusplus" == elems[0] )
        {
          mDataModel["type"]       = elems[0];
          mDataModel["classname"]  = elems[1];
          mDataModel["headerfile"] = elems[2];
        }
        else
        {
          YaFsm::printFatal( std::string( "invalid data model type" ) + elems[0] );
        }
      }
      else
      {
        YaFsm::printFatal( std::string( "invalid datamodel string" ) + str );
      }
    }
  }

  const tinyxml2::XMLElement* datamodelElem = elem->FirstChildElement( "datamodel" );
  if ( datamodelElem )
  {
    const tinyxml2::XMLElement* data = datamodelElem->FirstChildElement( "data" );
    while ( data )
    {
      const tinyxml2::XMLAttribute* id = data->FindAttribute( "id" );
      if ( id )
      {
        mMembers[id->Value()] = data;
      }
      data = data->NextSiblingElement( "data" );
    }
  }
}

void YaFsmScxmlParser::checkSubEvents( const tinyxml2::XMLElement* elem )
{
  if ( elem )
  {
    const tinyxml2::XMLElement* raise = elem->FirstChildElement( "raise" );
    while ( raise )
    {
      const char* event = raise->Attribute( "event" );
      if ( event )
      {
        mEvents[event] = 1;
      }
      raise = raise->NextSiblingElement( "raise" );
    }

    const tinyxml2::XMLElement* send = elem->FirstChildElement( "send" );
    while ( send )
    {
      const char* event = send->Attribute( "event" );
      if ( event )
      {
        mEvents[event] = 1;
      }
      send = send->NextSiblingElement( "send" );
    }
  }
}

void YaFsmScxmlParser::parseFSM( const tinyxml2::XMLElement* elem )
{
  const tinyxml2::XMLElement* final = elem->FirstChildElement( "final" );
  if ( final )
  {
    const char* id = final->Attribute( "id" );
    if ( id )
    {
      mStates[std::string( id )] = final;
    }
  }

  const tinyxml2::XMLElement* state = elem->FirstChildElement( "state" );
  while ( state )
  {
    const char* id = state->Attribute( "id" );
    if ( id )
    {
      mStates[std::string( id )] = state;
    }

    const tinyxml2::XMLElement* transition = state->FirstChildElement( "transition" );
    while ( transition )
    {
      const char* event = transition->Attribute( "event" );
      if ( event )
      {
        mTriggers[event] = 1;
      }
      else
      {
        YaFsm::printWarn( std::string( "transition on state " ) + id + std::string( " with empty trigger" ) );
      }

      checkSubEvents( transition );

      transition = transition->NextSiblingElement( "transition" );
    }

    const tinyxml2::XMLElement* onentry = state->FirstChildElement( "onentry" );
    if ( onentry )
      checkSubEvents( onentry );
    const tinyxml2::XMLElement* onexit = state->FirstChildElement( "onexit" );
    if ( onexit )
      checkSubEvents( onexit );

    if ( hasSubStates( state ) )
    {
      parseFSM( state );
    }
    state = state->NextSiblingElement( "state" );
  }
}

bool YaFsmScxmlParser::hasSubStates( const tinyxml2::XMLElement* elem )
{
  bool                        bRet  = false;
  const tinyxml2::XMLElement* final = elem->FirstChildElement( "final" );
  if ( final )
    bRet = true;

  const tinyxml2::XMLElement* state = elem->FirstChildElement( "state" );
  if ( state )
    bRet = true;

  return bRet;
}

std::string YaFsmScxmlParser::getEnterStateName( const tinyxml2::XMLElement* elem )
{
  std::string                 name;
  const tinyxml2::XMLElement* initial = elem->FirstChildElement( "initial" );
  if ( initial )
  {
    const tinyxml2::XMLElement* transition = initial->FirstChildElement( "transition" );
    if ( transition )
    {
      name = transition->Attribute( "target" );
    }
  }

  return name;
}

bool YaFsmScxmlParser::hasActions( const std::string& str, const tinyxml2::XMLElement* elem )
{
  bool                        has    = false;
  const tinyxml2::XMLElement* action = elem->FirstChildElement( str.c_str() );
  if ( action )
  {
    has = true;
  }

  return has;
}

void YaFsmScxmlParser::writeInterfaceFSMStateHeader()
{
  std::ofstream fh;
  fh.open( ( mCodeOutDir + YaFsm::sep + std::string( "I" ) + mDataModel["name"] + std::string( "State.h" ) ),
           std::ofstream::out | std::ofstream::trunc );
  fh << "#pragma once\n\n";
  auto it = mDataModel.find( "headerfile" );
  if ( it != mDataModel.end() )
  {
    fh << "#include \"" << mDataModel["headerfile"] << "\"\n";
  }

  std::string n = ns();

  std::string strNS;

  fh << "\n";
  if ( !n.empty() )
  {
    fh << "namespace " << n << "\n";
    fh << "{\n";
  }

  fh << "class " << mDataModel["name"] << ";\n";
  fh << "\n";
  fh << "class I" << mDataModel["name"] << "State\n";
  fh << "{\n";
  fh << "public:\n";
  fh << "  I" << mDataModel["name"] << "State() {}\n";
  fh << "  virtual ~I" << mDataModel["name"] << "State() {}\n";
  fh << "\n";
  fh << "  virtual void enter(" << mDataModel["name"] << "&) = 0;\n";
  fh << "  virtual void exit(" << mDataModel["name"] << "&) = 0;\n";
  fh << "\n";
  fh << "  // definiton of triggers\n";
  for ( auto it = mTriggers.begin(); it != mTriggers.end(); ++it )
  {
    fh << "  virtual void send_" << escapeNamespace( it->first ) << "( " << mDataModel["name"] << "&, const " << it->first << "& ) = 0;\n";
  }

  fh << "};\n";
  if ( !n.empty() )
  {
    fh << "}\n";
  }

  fh << "\n";
  fh.close();
}

void YaFsmScxmlParser::writeFSMStateBaseHeader()
{
  std::ofstream fh;
  fh.open( ( mCodeOutDir + YaFsm::sep + mDataModel["name"] + std::string( "StateBase.h" ) ), std::ofstream::out | std::ofstream::trunc );
  fh << "#pragma once";
  fh << "\n";
  fh << "#include \"I" << mDataModel["name"] << "State.h\"\n";

  fh << "#include <string>\n";
  fh << "#include <iostream>\n";
  fh << "#include <sstream>\n";

  fh << "\n";
  fh << "\n";

  std::string n = ns();

  fh << "\n";
  if ( !n.empty() )
  {
    fh << "namespace " << n << "\n";
    fh << "{\n";
  }

  std::string strNS;

  if ( !n.empty() )
  {
    strNS = n + std::string( "::" );
  }

  fh << "class " << mDataModel["name"] << "StateBase: public " << strNS << "I" << mDataModel["name"] << "State\n";

  fh << "{\n";
  fh << "  friend class " << mDataModel["name"] << ";\n";
  fh << "\n";
  fh << "public:\n";
  fh << "  " << mDataModel["name"] << "StateBase( )\n";
  fh << "   : mStateName()\n";
  fh << "   , mParentStateName()\n";
  fh << "    {\n";
  fh << "    }\n";

  fh << "  virtual ~" << mDataModel["name"] << "StateBase() {}\n";
  fh << "\n";
  fh << "protected:\n";
  fh << "  const std::string& getStateName() const;\n";
  fh << "  const std::string& getParentStateName() const;\n";
  fh << "  void setStateName(const std::string&);\n";
  fh << "  void setParentStateName(const std::string&);\n";
  fh << "\n";
  fh << "  // definition of all triggers\n";

  for ( auto it = mTriggers.begin(); it != mTriggers.end(); ++it )
  {
    fh << "  inline virtual void send_" << escapeNamespace( it->first ) << "( " << mDataModel["name"] << "&, const " << it->first << "& _event)\n";
    fh << "  {\n";
    fh << "  }\n";
  }

  fh << "\n";

  fh << "\n";
  fh << "protected:\n";
  fh << "  std::string mStateName;\n";
  fh << "  std::string mParentStateName;\n";

  fh << "};\n";
  fh << "\n";
  fh << "inline const std::string& " << mDataModel["name"] << "StateBase::getStateName() const\n";
  fh << "{\n";
  fh << "  return mStateName;\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "StateBase::setStateName(const std::string& str)\n";
  fh << "{\n";
  fh << "  mStateName = str;\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline const std::string& " << mDataModel["name"] << "StateBase::getParentStateName() const\n";
  fh << "{\n";
  fh << "  return mParentStateName;\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "StateBase::setParentStateName(const std::string& str)\n";
  fh << "{\n";
  fh << "  mParentStateName = str;\n";
  fh << "}\n";
  fh << "\n";

  if ( !n.empty() )
  {
    fh << "}\n";
  }

  fh.close();
}

void YaFsmScxmlParser::writeFSMStates( const tinyxml2::XMLElement* elem )
{
  std::ofstream fh;
  std::ofstream fs;
  fh.open( ( mCodeOutDir + YaFsm::sep + mDataModel["name"] + std::string( "StateImpl.h" ) ), std::ofstream::out | std::ofstream::trunc );
  fs.open( ( mCodeOutDir + YaFsm::sep + mDataModel["name"] + std::string( "StateImpl.cpp" ) ), std::ofstream::out | std::ofstream::trunc );
  fh << "#pragma once";

  fh << "\n";
  fh << "#include \"" << mDataModel["name"] << "StateBase.h\"\n";
  fh << "\n";
  fh << "// definitions of all States as classes\n";

  auto it = mDataModel.find( "classname" );
  if ( it != mDataModel.end() )
  {
    fh << "class " << mDataModel["classname"] << ";\n";
  }

  fh << "namespace N" << mDataModel["name"] << "\n";
  fh << "{\n";

  fs << "#include \"" << mDataModel["name"] << "StateImpl.h\"\n";
  fs << "#include \"" << mDataModel["name"] << ".h\"\n";
  fs << "#include <vector>\n";
  fs << "#include <iostream>\n";

  fs << "namespace N" << mDataModel["name"] << "\n";
  fs << "{\n";

  writeFSMStates( fh, fs, elem, "" );

  fh << "}\n";
  fs << "}\n";

  fh.close();
  fs.close();
}

void YaFsmScxmlParser::writeFSMStates( std::ofstream& fh, std::ofstream& fs, const tinyxml2::XMLElement* elem, const std::string& parentName )
{
  std::string                 keyword = "state";
  const tinyxml2::XMLElement* state   = elem->FirstChildElement( keyword.c_str() );

  while ( state )
  {
    if ( hasSubStates( state ) )
    {
      genStateImpl( fh, fs, state, parentName );
      writeFSMStates( fh, fs, state, state->Attribute( "id" ) );
    }
    else
    {
      genStateImpl( fh, fs, state, parentName );
    }

    genTransImpl( fh, fs, state, parentName );

    state = state->NextSiblingElement( keyword.c_str() );
    if ( !state && keyword == "state" )
    {
      keyword = "final";
      state   = elem->FirstChildElement( keyword.c_str() );
    }
  }
}

int YaFsmScxmlParser::delayToInt( const std::string& str ) const
{
  int         delay = 0;
  std::string value;

  if ( !str.empty() )
  {
    size_t idx = str.find( "ms" );
    if ( idx != std::string::npos )
    {
      value = str.substr( 0, idx );
      delay = std::stoi( value );
    }
    else
    {
      idx = str.find( "s" );
      if ( idx != std::string::npos )
      {
        value = str.substr( 0, idx );
        delay = std::stoi( value ) * 1000;
      }
    }
  }
  return delay;
}

void YaFsmScxmlParser::genStateActions( std::ofstream& fs, const std::string& state_id, const tinyxml2::XMLElement* elem )
{

  const tinyxml2::XMLElement* action = elem->FirstChildElement();
  while ( action )
  {
    if ( std::string( "script" ) == std::string( action->Name() ) )
    {
      std::string text = action->GetText();
      fs << "  " << text << "\n";
    }
    else if ( std::string( "raise" ) == std::string( action->Name() ) )
    {
      fs << "  fsmImpl.sendEvent(\"" << state_id << "\", " << action->Attribute( "event" ) << "(), 0);\n";
    }
    else if ( std::string( "send" ) == std::string( action->Name() ) )
    {
      std::string event = action->Attribute( "event" );
      fs << "  " << event << " data" << escapeNamespace( event ) << ";\n";
      const tinyxml2::XMLElement* para = action->FirstChildElement( "param" );
      if ( para )
      {
        if ( para->Attribute( "expr" ) )
        {
          fs << "  data" << escapeNamespace( event ) << "." << para->Attribute( "name" ) << " = " << para->Attribute( "expr" ) << ";\n";
        }
        para = para->NextSiblingElement( "para" );
      }

      if ( action->Attribute( "id" ) )
      {
        YaFsm::printWarn( "use of generated ids for send event not allowed!\n!" );
      }
      else if ( action->Attribute( "idlocation" ) )
      {
        YaFsm::printWarn( "use of idlocation for send event not allowed!\n!" );
      }
      int delay = 0;
      if ( action->Attribute( "delay" ) )
      {
        delay = delayToInt( action->Attribute( "delay" ) );
      }

      fs << "  "
         << "fsmImpl.sendEvent( \"" << state_id << "\", data" << escapeNamespace( event ) << ", " << delay << ");\n";
    }
    else if ( std::string( "cancel" ) == std::string( action->Name() ) )
    {
      fs << "    fsmImpl.cancelEvent(\"" << action->Attribute( "sendid" ) << "\");\n";
    }
    action = action->NextSiblingElement();
  }
}

void YaFsmScxmlParser::genTransitionActions( std::ofstream& fs, const std::string& event, const tinyxml2::XMLElement* elem )
{

  const tinyxml2::XMLElement* action = elem->FirstChildElement();
  while ( action )
  {
    if ( std::string( "script" ) == std::string( action->Name() ) )
    {
      if ( mVerbose )
        std::cout << "implement script action for transtion " << event << std::endl;
      std::string text = action->GetText();
      fs << "    " << text << "\n";
    }
    else if ( std::string( "raise" ) == std::string( action->Name() ) )
    {
      if ( mVerbose )
        std::cout << "implement raise action for transition " << event << std::endl;
      fs << "    fsmImpl.sendEvent(\"" << event << "\", " << action->Attribute( "event" ) << "(), 0);\n";
    }
    else if ( std::string( "assign" ) == std::string( action->Name() ) )
    {
      if ( mVerbose )
        std::cout << "unhandled action assign " << std::endl;
    }
    else if ( std::string( "send" ) == std::string( action->Name() ) )
    {
      if ( mVerbose )
        std::cout << "implement send action for transition " << event << std::endl;
      std::string event = action->Attribute( "event" );
      fs << "    " << event << " data" << escapeNamespace( event ) << ";\n";
      const tinyxml2::XMLElement* para = action->FirstChildElement( "param" );
      if ( para )
      {
        if ( para->Attribute( "expr" ) )
        {
          fs << "    data" << escapeNamespace( event ) << "." << para->Attribute( "name" ) << " = " << para->Attribute( "expr" ) << ";\n";
        }
        para = para->NextSiblingElement( "para" );
      }

      if ( action->Attribute( "id" ) )
      {
        YaFsm::printWarn( "use of generated ids for send event not allowed!\n!" );
      }
      else if ( action->Attribute( "idlocation" ) )
      {
        YaFsm::printWarn( "use of idlocation for send event not allowed!\n!" );
      }
      int delay = 0;
      if ( action->Attribute( "delay" ) )
      {
        delay = delayToInt( action->Attribute( "delay" ) );
      }

      fs << "    "
         << "fsmImpl.sendEvent( \"" << event << "\", data" << escapeNamespace( event ) << ", " << delay << ");\n";
    }
    else if ( std::string( "cancel" ) == std::string( action->Name() ) )
    {
      if ( mVerbose )
        std::cout << "implement cancel action for transition " << event << std::endl;

      fs << "    fsmImpl.cancelEvent(\"" << action->Attribute( "sendid" ) << "\");\n";
    }

    action = action->NextSiblingElement();
  }
}

void YaFsmScxmlParser::genStateImpl( std::ofstream& fh, std::ofstream& fs, const tinyxml2::XMLElement* state, const std::string& parentName )
{
  if ( !state )
  {
    return;
  }

  std::string state_id = state->Attribute( "id" );

  std::string strNS;

  if ( !ns().empty() )
  {
    strNS = ns() + std::string( "::" );
  }

  if ( !parentName.empty() )
  {
    fh << "class State" << state_id << ": public State" << parentName << "\n";
  }
  else
  {
    fh << "class State" << state_id << ": public " << strNS << mDataModel["name"] << "StateBase\n";
  }
  fh << "{\n";
  fh << "public:\n";
  fh << "  State" << state_id << "();\n";
  fh << "  virtual ~State" << state_id << "();\n";
  fh << "protected:\n";
  std::map<std::string, bool> processedTriggers;

  const tinyxml2::XMLElement* transition = state->FirstChildElement( "transition" );
  while ( transition )
  {
    const char* event = transition->Attribute( "event" );
    if ( event )
    {
      auto it = processedTriggers.find( event );
      if ( it == processedTriggers.end() )
      {
        //        const char* source = transition->Attribute("source");
        //        if( source && source == state_id)
        {
          fh << "  virtual void send_" << escapeNamespace( event ) << "( " << strNS << mDataModel["name"] << "&, const " << event << "& _event );\n";
          processedTriggers[event] = true;
        }
      }
    }

    transition = transition->NextSiblingElement( "transition" );
  }

  fh << "private:\n";
  fh << "  void enter( " << strNS << mDataModel["name"] << "& );\n";
  fh << "  void exit( " << strNS << mDataModel["name"] << "& );\n";

  transition = state->FirstChildElement( "transition" );
  //  while(transition)
  //  {
  //    const tinyxml2::XMLElement* script = transition->FirstChildElement("script");
  //    std::string actionName = std::string("transition_") + state_id + "_" + transition->Attribute("event") +"_" + std::to_string(transIdx);
  //    std::string event = transition->Attribute("event");
  //    auto it = mDataModel.find("classname");
  //    if(it != mDataModel.end())
  //    {
  //      if(!event.empty())
  //      {
  //        fh << "  void " << actionName << "( " <<  mDataModel["classname"] << "& model, const " << event << "& _event );\n";
  //        fs << "void State"<< state_id <<"::" << actionName << "( " <<  mDataModel["classname"] << "& model, const " << event << "& _event );\n";
  //      }
  //      else
  //      {
  //        fh << "  void " << actionName << "( " <<  mDataModel["classname"] << "& model );\n";
  //        fs << "  void " << actionName << "( " <<  mDataModel["classname"] << "& model )\n";
  //      }
  //    }
  //    else
  //    {
  //      fh << "  void " << actionName << "();\n";
  //      fs << "  void " << actionName << "()\n";
  //    }
  //    fs << "{\n";

  //    while(script)
  //    {
  //      std::string text = script->GetText();
  //      fs << "  " << text << "\n";
  //      script = script->NextSiblingElement("script");
  //    }

  //    fs << "}\n\n";

  //    transIdx++;

  //    transition = transition->NextSiblingElement("transition");

  //  }

  fh << "};\n\n";

  fs << "State" << state_id << "::State" << state_id << "()\n";
  fs << "{\n";
  fs << "  setStateName( \"" << state_id << "\" );\n";
  if ( !parentName.empty() )
  {
    fs << "  setParentStateName( \"" << parentName << "\" );\n";
  }
  fs << "}\n\n";

  fs << "State" << state_id << "::~State" << state_id << "()\n";
  fs << "{\n";
  fs << "}\n\n";

  std::string enterStateName = getEnterStateName( state );
  fs << "void State" << state_id << "::enter( " << strNS << mDataModel["name"] << "& fsmImpl )\n";

  fs << "{\n";

  auto it = mDataModel.find( "classname" );
  if ( it != mDataModel.end() )
  {
    fs << "  " << mDataModel["classname"] << "& model = fsmImpl.model();\n";
  }

  if ( hasActions( "onentry", state ) )
  {
    genStateActions( fs, state_id, state->FirstChildElement( "onentry" ) );
  }

  if ( !enterStateName.empty() )
  {
    fs << "  fsmImpl.setStateByName( \"" << enterStateName << "\" );\n";
    fs << "  fsmImpl.enterCurrentState();\n";
  }

  fs << "  std::ignore = ( model );\n";

  fs << "}\n\n";

  fs << "void State" << state_id << "::exit( " << strNS << mDataModel["name"] << "& fsmImpl )\n";

  fs << "{\n";

  if ( it != mDataModel.end() )
  {
    fs << "  " << mDataModel["classname"] << "& model = fsmImpl.model();\n";
  }

  if ( hasActions( "onexit", state ) )
  {
    genStateActions( fs, state_id, state->FirstChildElement( "onexit" ) );
  }
  else
  {
    fs << "  std::ignore = ( model );\n";
  }

  fs << "}\n\n";
}

void YaFsmScxmlParser::genTransImpl( std::ofstream& /*fh*/, std::ofstream& fs, const tinyxml2::XMLElement* state, const std::string& parentName )
{
  std::string strNS;

  if ( !ns().empty() )
  {
    strNS = ns() + std::string( "::" );
  }

  std::map<std::string, bool> processedTriggers;

  const tinyxml2::XMLElement* transition = state->FirstChildElement( "transition" );
  while ( transition )
  {
    std::string event = std::string( transition->Attribute( "event" ) );
    ///    std::string source = std::string(transition->Attribute("source"));
    std::string source = std::string( state->Attribute( "id" ) );
    std::string target = source;
    if ( transition->Attribute( "target" ) )
    {
      target = transition->Attribute( "target" );
    }
    std::string criteria = source + "_" + event;
    //    # add methods for each trigger.
    //    # be carefull that a trigger is not implemented twice, depending on conditions
    //    # so consider trigger name and begin of a trigger as condition if already implemented
    auto it = processedTriggers.find( criteria );
    if ( !event.empty() && it == processedTriggers.end() )
    {
      processedTriggers[criteria] = true;
      fs << "void State" << source << "::send_" << escapeNamespace( event ) << "(" << strNS << mDataModel["name"] << "& fsmImpl, const " << event
         << "& _event)\n";
      fs << "{\n";

      fs << "  (void) fsmImpl;\n";
      auto it = mDataModel.find( "classname" );
      if ( it != mDataModel.end() )
      {
        fs << "  " << mDataModel["classname"] << "& model = fsmImpl.model();\n";
        fs << "  "
           << "std::ignore = ( model );\n";
      }

      fs << "\n";

      // iterate a second time for identical triggers with different conditions or triggers without conditions
      const tinyxml2::XMLElement* transition2 = state->FirstChildElement( "transition" );
      std::map<std::string, bool> processedConditions;

      while ( transition2 )
      {
        std::string event2    = std::string( transition2->Attribute( "event" ) );
        std::string source2   = std::string( state->Attribute( "id" ) );
        std::string criteria2 = source2 + "_" + event2;
        std::string condition2;
        if ( transition2->Attribute( "cond" ) )
        {
          condition2 = transition2->Attribute( "cond" );
        }

        auto itCondition = processedConditions.begin();
        if ( !condition2.empty() )
        {
          itCondition = processedConditions.find( condition2 );
        }

        if ( criteria2 == criteria && ( condition2.empty() || itCondition == processedConditions.end() ) )
        {
          if ( !condition2.empty() )
          {
            processedConditions[condition2] = true;
          }

          std::string target2 = source2;
          if ( transition2->Attribute( "target" ) )
          {
            target2 = transition2->Attribute( "target" );
          }

          if ( !condition2.empty() )
          {
            fs << "  if( " << condition2 << " )\n";
          }
          fs << "  {\n";

          std::string transName = criteria2 + "(" + condition2 + ")";

          fs << "    fsmImpl.setTransByName(\"" << transName << "\");\n";
          mTransitions[transName] = transition;
          // current could be difficult to determine. if we made a fallthrough into next hierarchy level
          // exit state by name
          if ( source2 != target2 )
          {
            fs << "    fsmImpl.exitState(\"" << source2 << "\");\n";
          }

          genTransitionActions( fs, event2, transition2 );

          if ( source2 != target2 )
          {
            fs << "    fsmImpl.setStateByName(\"" << target2 << "\");\n";
            fs << "    fsmImpl.enterCurrentState();\n";
          }

          if ( !condition2.empty() && !parentName.empty() )
          {
            fs << "  }\n";
            fs << "  else // condition is not matched, we should now try if condition is matched by parent\n";
            fs << "  {\n";

            fs << "    State" << parentName << "::send_" << escapeNamespace( event2 ) << "( fsmImpl, _event );\n";
          }

          fs << "  }\n\n";
        }
        transition2 = transition2->NextSiblingElement( "transition" );
      }

      fs << "}\n\n";
    }
    transition = transition->NextSiblingElement( "transition" );
  }
}

void YaFsmScxmlParser::writeFSMHeader( const tinyxml2::XMLElement* elem )
{
  std::ofstream fh;
  fh.open( ( mCodeOutDir + YaFsm::sep + mDataModel["name"] + std::string( ".h" ) ), std::ofstream::out | std::ofstream::trunc );
  fh << "#pragma once";
  fh << "\n";

  fh << "#include \"" << mDataModel["name"] << "StateImpl.h\"\n";

  auto it = mDataModel.find( "headerfile" );
  if ( it != mDataModel.end() )
  {
    fh << "#include \"" << mDataModel["headerfile"] << "\"\n";
  }

  for ( auto it = mMembers.begin(); it != mMembers.end(); ++it )
  {
    const char* src = ( *it ).second->Attribute( "src" );
    if ( src )
    {
      fh << "#include \"" << src << "\"\n";
    }
  }

  fh << "#include \"IScxmlFSMEventCB.h\"\n";
  fh << "#include \"IScxmlFSMEvent.h\"\n";
  fh << "#include \"ScxmlFSMEvent.h\"\n";

  fh << "\n";
  fh << "\n";

  fh << "#include <string>\n";
  fh << "#include <map>\n";
  fh << "#include <vector>\n";
  fh << "#include <assert.h>\n";

  fh << "\n// forward declarations\n";

  std::string strNS;

  if ( !ns().empty() )
  {
    strNS = ns() + std::string( "::" );
  }

  if ( !ns().empty() )
  {
    fh << "namespace " << ns() + "\n";
    fh << "{\n";
  }

  fh << "class " << mDataModel["name"] << "StateBase;\n";
  if ( !ns().empty() )
  {
    fh << "}\n";
  }

  if ( !ns().empty() )
  {
    fh << "namespace " << ns() + "\n";
    fh << "{\n";
  }

  fh << "\nclass " << mDataModel["name"] << "\n";
  fh << " : public IScxmlFSMEventCB\n";
  fh << "{\n";
  fh << "  friend class " << mDataModel["name"] << "StateBase;\n";

  for ( auto it = mStates.begin(); it != mStates.end(); ++it )
  {
    std::string id = ( *it ).first;
    fh << "  friend class N" << mDataModel["name"] << "::State" << id << ";\n";
  }

  fh << "\n";
  fh << "public:\n";
  fh << "  " << mDataModel["name"] << "()\n";
  fh << "  : mpoCurrentState( 0 )\n";

  it = mDataModel.find( "classname" );
  if ( it != mDataModel.end() )
  {
    fh << "  , mDataModel()\n";
  }

  fh << "  , mbLockTrigger( false )\n";
  fh << "  , mbInit( false )\n";
  fh << "  , mFSMEvent( self() )\n";

  // todo implement member handling
  for ( auto it = mMembers.begin(); it != mMembers.end(); ++it )
  {
    //    const char* src = (*it).second->Attribute("src");
    //    const char* expr = (*it).second->Attribute("expr");
    //    const char* id = (*it).second->Attribute("expr");
    //    if( src )
    //    {
    //      fh << "  , " << (*it).second->Attribute("id") << "()\n";
    //    }
    //    else if (expr)
    //    {
    //      fh << "  , " << (*it).second->Attribute("id") << "(" << expr << ")\n";
    //    }
  }
  fh << "  {\n";

  for ( auto it = mStates.begin(); it != mStates.end(); ++it )
  {
    std::string id = ( *it ).first;
    fh << "    moStateMap[\"" << id << "\"] = &moState" << id << ";\n";
    fh << "    moStateCoverageMap[\"" << id << "\"] = 0;\n";
  }

  for ( auto it = mTransitions.begin(); it != mTransitions.end(); ++it )
  {
    std::string id = ( *it ).first;
    fh << "    moTransitionCoverageMap[\"" << id << "\"] = 0;\n";
  }

  for ( auto it = mEvents.begin(); it != mEvents.end(); ++it )
  {
    fh << "    mFSMEvent.setEventID( EVENT_" << escapeNamespace( ( *it ).first ) << ");\n";
  }

  YaFsm::printDbg( "get default enter state name for mpoCurrentState" );
  const char* initial = elem->Attribute( "initial" );
  if ( initial )
  {
    fh << "    setStateByName(\"" << initial << "\");\n";
  }
  else
  {
    YaFsm::printFatal( "required enter state missing" );
  }

  fh << "  }\n";
  fh << "  virtual ~" << mDataModel["name"] << "() {}\n";
  fh << "\n";

  fh << "  void initFSM( void );\n";
  for ( auto it = mEvents.begin(); it != mEvents.end(); ++it )
  {
    fh << "  virtual int sendEvent( const std::string& sendId, const " << ( *it ).first << "& data, int iDelayMs);\n";
  }

  fh << "  virtual void cancelEvent( const std::string& sendId );\n";

  it = mDataModel.find( "classname" );
  if ( it != mDataModel.end() )
  {
    fh << "  " << mDataModel["classname"] << "& model() { return mDataModel; }\n";
    fh << "  const " << mDataModel["classname"] << "& model() const { return mDataModel; }\n";
  }

  fh << "  // definiton of triggers\n";
  for ( auto it = mTriggers.begin(); it != mTriggers.end(); ++it )
  {
    fh << "public:\n";
    fh << "  void sendEvent( const " << ( *it ).first << "& );\n";
  }

  fh << "\npublic:\n";

  fh << "// for getting statistics information\n";
  fh << "  void dumpCoverage( void ) const;\n";

  fh << "\n  protected:\n";

  fh << "  void setStateByName( const std::string& name );\n";
  fh << "  void setTransByName( const std::string& name );\n";
  fh << "  void enterCurrentState();\n";
  fh << "  void exitState( const std::string& name );\n";
  fh << "  virtual void processTimerEventID( int event, int id );\n";

  fh << "\n";
  fh << "//todo make this private and allow test makros to access this\n";

  fh << "#ifdef TESTFSM\n";
  fh << " public: const std::string& getStateName() const;\n";
  fh << "#else\n";
  fh << "  const std::string& getStateName() const;\n";
  fh << "#endif\n";

  // define events enumeration
  fh << "  public:\n";
  std::string name = mDataModel["name"];
  for ( auto& c : name )
    c = toupper( c );
  fh << "  enum " << name << "EVENT\n";
  fh << "  {\n";

  int enumIdx = 0;

  for ( auto it = mEvents.begin(); it != mEvents.end(); ++it )
  {
    if ( 0 == enumIdx )
    {
      fh << "    EVENT_" << escapeNamespace( ( *it ).first ) << "=1,\n";
    }
    else
    {
      fh << "    EVENT_" << escapeNamespace( ( *it ).first ) << ",\n";
    }
    enumIdx++;
  }
  fh << "  };\n";

  fh << "  private:\n";
  fh << "  //" << mDataModel["name"] << "StateBase& ( const std::string& name );\n";
  fh << "  " << mDataModel["name"] << "& self();\n";
  fh << "  bool isLocked( void );\n";
  fh << "  bool isInitialised( void );\n";
  fh << "  void registerEventID( int );\n";

  fh << R"-(
  class Locker
  {
  public:
    Locker(bool& lock)
    :mLock(lock)
    {
      mLock = true;
    }
  
    ~Locker()
    {
      mLock = false;
    }
  
  private:
    bool& mLock;
  };

)-";

  fh << "  void exitSubStates(const std::string& name );\n";

  fh << "  " << mDataModel["name"] << "StateBase* mpoCurrentState;\n";

  it = mDataModel.find( "classname" );
  if ( it != mDataModel.end() )
  {
    fh << "  " << mDataModel["classname"] << " mDataModel;\n";
  }

  fh << "\n";
  fh << "  // definition of all states as members\n";
  for ( auto it = mStates.begin(); it != mStates.end(); ++it )
  {
    std::string id = ( *it ).first;
    fh << "  N" << mDataModel["name"] << "::State" << id << " moState" << id << ";\n";
  }
  fh << "  std::map<std::string, " << mDataModel["name"] << "StateBase*> moStateMap;\n";
  fh << "  std::map<std::string, int> moStateCoverageMap;\n";
  fh << "  std::map<std::string, int> moTransitionCoverageMap;\n";
  fh << "  bool mbLockTrigger;\n";
  fh << "  bool mbInit;\n";
  fh << "  ScxmlFSMEvent mFSMEvent;\n";
  for ( auto it = mEvents.begin(); it != mEvents.end(); ++it )
  {
    fh << "  std::map<int, " << ( *it ).first << "> mParaMap_" << escapeNamespace( ( *it ).first ) << ";\n";
  }

  // there seems no better way, variants could be used instead.
  // non necessary to handle members, they could also be part of the model.

  for ( auto it = mMembers.begin(); it != mMembers.end(); ++it )
  {
    // const char* src = (*it).second->Attribute("src");
    // const char* classname = (*it).second->Attribute("classname");
    const char* id   = ( *it ).second->Attribute( "id" );
    const char* expr = ( *it ).second->Attribute( "expr" );

    if ( id && expr )
    {
      fh << "  static const auto " << id << " = " << expr << ";\n";
    }
  }
  fh << "\n";
  fh << "};\n";
  fh << "\n";
  fh << "inline " << mDataModel["name"] << "& " << mDataModel["name"] << "::self()\n";
  fh << "{\n";
  fh << "  return (*this);\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "::setStateByName( const std::string& name)\n";
  fh << "{\n";
  fh << "  // set states by names\n";
  fh << "  if (moStateMap.end() != moStateMap.find(name) )\n";
  fh << "  {\n";
  fh << "    mpoCurrentState =  moStateMap[name];\n";
  fh << "    moStateCoverageMap[name] = moStateCoverageMap[name] + 1;\n";
  fh << "  }\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "::setTransByName( const std::string& name)\n";
  fh << "{\n";
  fh << "  // set transitions by names\n";
  fh << "  if (moTransitionCoverageMap.end() != moTransitionCoverageMap.find(name) )\n";
  fh << "  {\n";
  fh << "    moTransitionCoverageMap[name] = moTransitionCoverageMap[name] + 1;\n";
  fh << "  }\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline const std::string& " << mDataModel["name"] << "::getStateName() const\n";
  fh << "{\n";
  fh << "  return mpoCurrentState->getStateName();\n";
  fh << "\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "::enterCurrentState()\n";
  fh << "{\n";
  fh << "  mpoCurrentState->enter(self());\n";
  fh << "}\n";
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "::exitState( const std::string& name )\n";
  fh << "{\n";
  fh << "  exitSubStates(name);\n";
  fh << "  moStateMap[name]->exit(self());\n";
  fh << "}\n\n";
  fh << "inline void " << mDataModel["name"] << "::exitSubStates( const std::string& name )\n";
  fh << "{\n";
  fh << "  std::vector<std::string> subStates;\n";
  fh << "  std::string stateName = mpoCurrentState->getStateName();\n";
  fh << "  while(name != stateName)\n";
  fh << "  {\n";
  fh << "    moStateMap[stateName]->exit(self());\n";
  fh << "    stateName = moStateMap[stateName]->getParentStateName();\n";
  fh << "  }\n";
  fh << "}\n\n";

  fh << "inline bool " << mDataModel["name"] << "::isLocked( void )\n";
  fh << "{\n";
  fh << "  return mbLockTrigger;\n";
  fh << "}\n\n";
  fh << "inline bool " << mDataModel["name"] << "::isInitialised( void )\n";
  fh << "{\n";
  fh << "  return mbInit;\n";
  fh << "}\n\n";
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "::initFSM( void )\n";
  fh << "{\n";
  fh << "  mbInit = true;\n";
  fh << "  enterCurrentState();\n";
  fh << "}\n\n";
  fh << "\n";

  for ( auto it = mEvents.begin(); it != mEvents.end(); ++it )
  {
    fh << "inline int " << mDataModel["name"] << "::sendEvent( const std::string& sendId, const " << ( *it ).first << "& data, int iDelayMs )\n";
    fh << "{\n";
    fh << "  int id = mFSMEvent.sendEventID( sendId + \"." << ( *it ).first << "\", EVENT_" << escapeNamespace( ( *it ).first ) << " , iDelayMs );\n";
    fh << "  mParaMap_" << escapeNamespace( ( *it ).first ) << "[id] = data;\n";
    fh << "  return id;\n";
    fh << "}\n";
  }

  fh << "inline void " << mDataModel["name"] << "::cancelEvent( const std::string& sendId )\n";
  fh << "{\n";
  fh << "  std::vector<int> ids = mFSMEvent.cancelEvent( sendId );\n";
  fh << "  for(auto it = ids.begin(); it != ids.end(); ++it )\n";
  fh << "  {\n";

  for ( auto it = mEvents.begin(); it != mEvents.end(); ++it )
  {
    std::string mapName = escapeNamespace( ( *it ).first );
    fh << "    if( mParaMap_" << mapName << ".find(*it) != mParaMap_" << mapName << ".end()) { mParaMap_" << mapName << ".erase(*it); }\n";
  }
  fh << "  }\n";
  fh << "}\n";

  fh << "inline void " << mDataModel["name"] << "::processTimerEventID( int event, int id )\n";
  fh << "{\n";

  if ( !mEvents.empty() )

  {
    // definition of all timers as enumeration
    fh << "\n";
    fh << "  switch(event)\n";
    fh << "  {\n";
    for ( auto it = mEvents.begin(); it != mEvents.end(); ++it )
    {
      fh << "  case EVENT_" << escapeNamespace( ( *it ).first ) << ":\n";
      fh << "  {\n";
      fh << "    " << ( *it ).first << " event = mParaMap_" << escapeNamespace( ( *it ).first ) << "[id];\n";
      fh << "    sendEvent(event);\n";
      fh << "    mParaMap_" << escapeNamespace( ( *it ).first ) << ".erase(id);\n";
      fh << "  }\n";
      fh << "  break;\n";
    }

    fh << "  default:\n";
    fh << "  break;\n";
    fh << "  }\n";
  }

  fh << "}\n";
  fh << "\n";

  for ( auto it = mTriggers.begin(); it != mTriggers.end(); ++it )
  {
    fh << "inline void " << mDataModel["name"] << "::sendEvent( const " << ( *it ).first << "& _event)\n";

    fh << "{\n";
    fh << "  if( 0 != mpoCurrentState )\n";
    fh << "  {\n";
    fh << "    if( isInitialised() )\n";
    fh << "    {\n";
    fh << "      if( !isLocked() )\n";
    fh << "      {\n";
    fh << "        Locker lock( mbLockTrigger );\n";
    fh << "        mpoCurrentState->send_" << escapeNamespace( ( *it ).first ) << "( self(), _event );\n";
    fh << "      }\n";
    fh << "      else\n";
    fh << "      {\n";
    fh << "        std::cerr << ( \"forbidden call to trigger " << ( *it ).first << " from action\" ) << std::endl;\n";
    fh << "      }\n";
    fh << "    }\n";
    fh << "    else\n";
    fh << "    {\n";
    fh << "      std::cerr << ( \"call to trigger " << ( *it ).first << " before initFSM()\" ) << std::endl;\n";
    fh << "    }\n";

    fh << "  }\n";
    fh << "}\n";
    fh << "\n";
  }
  fh << "\n";
  fh << "inline void " << mDataModel["name"] << "::dumpCoverage( void ) const\n";
  fh << "{\n";
  fh << "  int iStatesCovered = 0;\n";
  fh << "  int iTransCovered = 0;\n";
  fh << "  std::cout << std::endl << \"Dumping coverage information:\" << std::endl;\n";
  fh << "  std::map<std::string, int>::const_iterator it;\n";
  fh << "  for(it = moStateCoverageMap.begin(); it != moStateCoverageMap.end(); ++it)\n";
  fh << "  {\n";
  fh << "    std::cout << \"  state\" << it->first << \" covered \"<< it->second << \" times\" << std::endl;\n";
  fh << "    if(0 < it->second)\n";
  fh << "    {\n";
  fh << "      iStatesCovered++;\n";
  fh << "    }\n";
  fh << "  }\n";

  fh << "  for(it = moTransitionCoverageMap.begin(); it != moTransitionCoverageMap.end(); ++it)\n";
  fh << "  {\n";
  fh << "    std::cout << \"  transition \" << it->first << \" covered \"<< it->second << \" times\" << std::endl;\n";
  fh << "    if(0 < it->second)\n";
  fh << "    {\n";
  fh << "      iTransCovered++;\n";
  fh << "    }\n";
  fh << "  }\n";

  fh << "  std::cout << std::endl << \"  total coverage:\" << std::endl;\n";
  fh << "  std::cout << \"  States covered: \" << iStatesCovered << \" out of \"<< moStateCoverageMap.size() << \", \";\n";
  fh << "  std::cout << (static_cast<size_t>(iStatesCovered)*1.0) / (1.0*moStateCoverageMap.size()) * 100.0 << \" percent\" << std::endl;\n";
  fh << "  std::cout << \"  Transitions covered: \" << iTransCovered << \" out of \"<< moTransitionCoverageMap.size() << \", \";\n";
  fh << "  std::cout << (static_cast<size_t>(iTransCovered)*1.0) / (1.0*moTransitionCoverageMap.size()) * 100.0 << \" percent\" << std::endl;\n";

  fh << "  \n";
  fh << "  \n";
  fh << "  \n";
  fh << "}\n";

  fh << "\n";

  if ( !ns().empty() )
  {
    fh << "}\n";
  }

  fh.close();
}

std::string YaFsmScxmlParser::escapeNamespace( std::string str ) const
{
  std::replace( str.begin(), str.end(), ':', '_' );
  return str;
}
