#include "YaFsmScxmlParser.h"
#include "YaFsm.h"
#include <iostream>
#include <fstream>

YaFsmScxmlParser::YaFsmScxmlParser()
{
}
std::string YaFsmScxmlParser::fileName() const
{
  return mFileName;
}

void YaFsmScxmlParser::setFileName(const std::string &fileName)
{
  mFileName = fileName;
}

std::string YaFsmScxmlParser::codeOutDir() const
{
  return mCodeOutDir;
}

void YaFsmScxmlParser::setCodeOutDir(const std::string &codeOutDir)
{
  mCodeOutDir = codeOutDir;
}

void YaFsmScxmlParser::init()
{

}

void YaFsmScxmlParser::readFSM()
{

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError error  = doc.LoadFile( mFileName.c_str() );
  if( tinyxml2::XML_SUCCESS == error )
  {
    tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
    if(elem)
    {
      parseDefinitions(elem);
      parseFSM(elem);

      // start writing out code files
      writeInterfaceFSMStateHeader();
      writeFSMStateBaseHeader();
      writeFSMStates(elem);
    }
  }
  else
  {
    YaFsm::printFatal("cannot read xml file " + mFileName);
  }

}

void YaFsmScxmlParser::parseDefinitions(const tinyxml2::XMLElement* elem)
{

  const tinyxml2::XMLAttribute* datamodel = elem->FindAttribute("datamodel");
  if( datamodel )
  {
    std::string str = datamodel->Value();
    if(!str.empty())
    {
       std::vector<std::string> elems = YaFsm::split(str, ':');
       if( 3 == elems.size())
       {
          if ("cplusplus" == elems[0])
          {
            mDataModel["type"] = elems[0];
            mDataModel["classname"] = elems[1];
            mDataModel["headerfile"] = elems[2];
          }
          else
          {
            YaFsm::printFatal(std::string("invalid data model type")+elems[0] );
          }
       }
       else
       {
         YaFsm::printFatal(std::string("invalid datamodel string") +  str);
       }

    }
  }

  const tinyxml2::XMLAttribute* name = elem->FindAttribute("name");
  if( name )
  {
    std::string str = name->Value();
    if(!str.empty())
    {
      mDataModel["name"] = str;
    }
    else
    {
      YaFsm::printFatal(std::string("empty name in scxml definition"));
    }
  }
  else
  {
    YaFsm::printFatal(std::string("no name attribute in scxml definition"));
  }

  const tinyxml2::XMLElement* datamodelElem = elem->FirstChildElement("datamodel");
  if(datamodelElem)
  {
    const tinyxml2::XMLElement* data = datamodelElem->FirstChildElement("data");
    while( data )
    {
      const tinyxml2::XMLAttribute* id = data->FindAttribute("id");
      if( id )
      {
        mMembers[id->Value()] = data;
      }
      data = data->NextSiblingElement("data");
    }
  }
}

void YaFsmScxmlParser::checkSubEvents( const tinyxml2::XMLElement* elem)
{
  if(elem)
  {
    const tinyxml2::XMLElement* raise = elem->FirstChildElement("raise");
    while( raise )
    {
      const char* event = raise->Attribute("event");
      if( event )
      {
        mEvents[event] = 1;
      }
      raise = raise->NextSiblingElement("raise");
    }

    const tinyxml2::XMLElement* send = elem->FirstChildElement("send");
    while( send )
    {
      const char* event = send->Attribute("event");
      if( event )
      {
        mEvents[event] = 1;
      }
      send = send->NextSiblingElement("send");
    }
  }
}

void YaFsmScxmlParser::parseFSM( const tinyxml2::XMLElement* elem )
{
  const tinyxml2::XMLElement* final = elem->FirstChildElement( "final" );
  if( final )
  {
    const char* id = final->Attribute("id");
    if (id)
    {
      mStates[std::string(id)] = final;
    }
  }

  const tinyxml2::XMLElement* state = elem->FirstChildElement( "state" );
  while(state)
  {
    const char* id = state->Attribute("id");
    if( id )
    {
      mStates[std::string(id)] = state;
    }

    const tinyxml2::XMLElement* transition = state->FirstChildElement("transition");
    while( transition)
    {
      const char* event = transition->Attribute("event");
      if(event)
      {
        mTriggers[event] = 1;
      }
      else
      {
        YaFsm::printWarn(std::string("transition on state ") + id + std::string(" with empty trigger"));
      }

      checkSubEvents(transition);

      transition = transition->NextSiblingElement("transition");
    }

    const tinyxml2::XMLElement* onentry = state->FirstChildElement("onentry");
    if(onentry) checkSubEvents(onentry);
    const tinyxml2::XMLElement* onexit = state->FirstChildElement("onexit");
    if(onexit) checkSubEvents(onexit);

    if(hasSubStates(state))
    {
      parseFSM(state);
    }
    state = state->NextSiblingElement("state");
  }
}

bool YaFsmScxmlParser::hasSubStates(const tinyxml2::XMLElement* elem)
{
  bool bRet = false;
  const tinyxml2::XMLElement* final = elem->FirstChildElement( "final" );
  if( final ) bRet = true;

  const tinyxml2::XMLElement* state = elem->FirstChildElement( "state" );
  if( state ) bRet = true;

  return bRet;
}

void YaFsmScxmlParser::writeInterfaceFSMStateHeader()
{
  std::ofstream fh;
  fh.open((mCodeOutDir + YaFsm::sep + std::string("I") + mDataModel["name"] + std::string("State.h")), std::ofstream::out | std::ofstream::trunc);
  fh << "#pragma once";
  auto it = mDataModel.find("headerfile");
  if( it != mDataModel.end())
  {
    fh << "#include \"" << mDataModel["headerfile"] << "\"\n";
  }

  fh << "\n";
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
  for( auto it = mTriggers.begin(); it != mTriggers.end(); ++it )
  {
    fh << "  virtual void send_" << it->first << "( " << mDataModel["name"] << "&, const " << it->first << "& ) = 0;\n";
  }


  fh << "};\n";
  fh << "\n";
  fh << "#endif\n";
  fh.close();

}


void YaFsmScxmlParser::writeFSMStateBaseHeader()
{
  std::ofstream fh;
  fh.open((mCodeOutDir + YaFsm::sep + mDataModel["name"] + std::string("StateBase.h")), std::ofstream::out | std::ofstream::trunc);
  fh << "#pragma once";
  fh << "\n";
  fh << "#include \"I" << mDataModel["name"] << "State.h\"\n";

  fh << "#include <string>\n";
  fh << "#include <iostream>\n";
  fh << "#include <sstream>\n";


  fh << "\n";
  fh << "\n";

  fh << "class " << mDataModel["name"] << "StateBase: public I" << mDataModel["name"] << "State\n";

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

  for( auto it = mTriggers.begin(); it != mTriggers.end(); ++it)
  {
    fh << "  inline virtual void send_" << it->first <<"( " << mDataModel["name"] << "&, const " << it->first << "& _event)\n";
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

  fh.close();

}


void YaFsmScxmlParser::writeFSMStates(const tinyxml2::XMLElement* elem)
{
  std::ofstream fh;
  std::ofstream fs;
  fh.open((mCodeOutDir + YaFsm::sep + mDataModel["name"] + std::string("StateImpl.h")), std::ofstream::out | std::ofstream::trunc);
  fs.open((mCodeOutDir + YaFsm::sep + mDataModel["name"] + std::string("StateImpl.cpp")), std::ofstream::out | std::ofstream::trunc);
  fh << "#pragma once";


  fh << "\n";
  fh << "#include \"" << mDataModel["name"] << "StateBase.h\"\n";
  fh << "\n";
  fh << "// definitions of all States as classes\n";

  auto it = mDataModel.find("classname");
  if( it != mDataModel.end())
  {
    fh << "class " << mDataModel["classname"] << ";\n";
  }

  fh << "namespace N"<< mDataModel["name"] << "\n";
  fh << "{\n";


  fs << "#include \"" << mDataModel["name"] << "StateImpl.h\"\n";
  fs << "#include \"" << mDataModel["name"] << ".h\"\n";
  fs << "#include <vector>\n";
  fs << "#include <iostream>\n";

  fs << "namespace N"<< mDataModel["name"] << "\n";
  fs << "{\n";

  writeFSMStates(fh,fs,elem,"");

  fh << "}\n";
  fs << "}\n";

  fh.close();
  fs.close();
}

void YaFsmScxmlParser::writeFSMStates(std::ofstream& fh, std::ofstream& fs, const tinyxml2::XMLElement* elem,const std::string& parentName)
{
  std::string keyword = "state";
  const tinyxml2::XMLElement* state = elem->FirstChildElement( keyword.c_str() );

  while(state)
  {
    if(hasSubStates(state))
    {
      genStateImpl(fh, fs, state, parentName);
      writeFSMStates(fh, fs, state, state->Attribute("id") );
    }
    else
    {
      genStateImpl(fh, fs, state, parentName );
    }
    //genStateTransImpl(fh, fs, state );

    state = state->NextSiblingElement(keyword.c_str());
    if(!state && keyword == "state")
    {
      keyword = "final";
      state = elem->FirstChildElement(keyword.c_str());
      if(state)
      {
        if( mVerbose ) std::cout << "found final state " << state->Attribute(("id")) <<std::endl;
      }
    }
  }
}

void YaFsmScxmlParser::genStateImpl(std::ofstream& fh, std::ofstream& fs, const tinyxml2::XMLElement* state, const std::string& parentName)
{
  if( !state)
  {
    return;
  }

  std::string state_id = state->Attribute("id");
  if( mVerbose ) std::cout << "generating code for state " << state_id << std::endl;


  if(!parentName.empty())
  {
    fh << "class State"<< state_id <<": public State" << parentName <<"\n";
  }
  else
  {
    fh << "class State"<< state_id <<": public " << mDataModel["name"] << "StateBase\n";
  }
  fh << "{\n";
  fh << "public:\n";
  fh << "  State" << state_id << "();\n";
  fh << "  virtual ~State" << state_id << "();\n";
  fh << "protected:\n";
  std::map<std::string, bool> processedTriggers;

  const tinyxml2::XMLElement* transition = state->FirstChildElement("transition");
  while( transition)
  {
    const char* event = transition->Attribute("event");
    if(event)
    {
      auto it = processedTriggers.find(event);
      if( it == processedTriggers.end())
      {
        const char* source = transition->Attribute("source");
        if( source && source == state_id)
        {
          fh << "  virtual void send_" << event << "( " << mDataModel["name"] << "&, const " << event <<"& _event );\n";
          processedTriggers[event] = true;
        }
      }
    }

    transition = transition->NextSiblingElement("transition");
  }

  fh << "private:\n";
  fh << "  void enter( " << mDataModel["name"] << "& );\n";
  fh << "  void exit( " << mDataModel["name"] << "& );\n";

  transition = state->FirstChildElement("transition");
  int transIdx = 0;
  while(transition)
  {
    const tinyxml2::XMLElement* script = transition->FirstChildElement("script");
    std::string actionName = std::string("transition_") + state_id + "_" + transition->Attribute("event") +"_" + std::to_string(transIdx);
    std::string event = transition->Attribute("event");
    auto it = mDataModel.find("classname");
    if(it != mDataModel.end())
    {
      if(!event.empty())
      {
        fh << "  void " << actionName << "( " <<  mDataModel["classname"] << "& model, const " << event << "& _event );\n";
        fs << "void State"<< state_id <<"::" << actionName << "( " <<  mDataModel["classname"] << "& model, const " << event << "& _event );\n";
      }
      else
      {
        fh << "  void " << actionName << "( " <<  mDataModel["classname"] << "& model );\n";
        fs << "  void " << actionName << "( " <<  mDataModel["classname"] << "& model )\n";
      }
    }
    else
    {
      fh << "  void " << actionName << "();\n";
      fs << "  void " << actionName << "()\n";
    }
    fs << "{\n";

    while(script)
    {
      fs << script << "\n";
      script = script->NextSiblingElement("script");
    }

    fs << "}\n\n";

    transIdx++;

    transition = transition->NextSiblingElement("transition");

  }

  const tinyxml2::XMLElement* onentry = state->FirstChildElement("onentry");
  if(onentry)
  {
    auto it = mDataModel.find("classname");
    if(it != mDataModel.end())
    {
      fh << "  void " << state_id << "_onEntry( " <<  mDataModel["classname"] << "& model );\n";
      fs << "  void " << state_id << "_onEntry( " <<  mDataModel["classname"] << "& model )\n";
    }
    else
    {
      fh << "  void " << state_id << "_onEntry();\n";
      fs << "  void " << state_id << "_onEntry()\n";
    }

    fs << "{\n";
    const tinyxml2::XMLElement* script = onentry->FirstChildElement("script");
    while(script)
    {
      fs << script << "\n";
      script = script->NextSiblingElement("script");
    }
    fs << "}\n";
  }

  const tinyxml2::XMLElement* onexit = state->FirstChildElement("onexit");
  if(onexit)
  {
    auto it = mDataModel.find("classname");
    if(it != mDataModel.end())
    {
      fh << "  void " << state_id << "_onExit( " <<  mDataModel["classname"] << "& model );\n";
      fs << "  void " << state_id << "_onExit( " <<  mDataModel["classname"] << "& model )\n";
    }
    else
    {
      fh << "  void " << state_id << "_onExit();\n";
      fs << "  void " << state_id << "_onExit()\n";
    }

    fs << "{\n";
    const tinyxml2::XMLElement* script = onexit->FirstChildElement("script");
    while(script)
    {
      fs << script << "\n";
      script = script->NextSiblingElement("script");
    }
    fs << "}\n";
  }



  fh << "};\n\n";

  fs << "State"<< state_id <<"::State"<< state_id <<"()\n";
  fs << "{\n";
  fs << "  setStateName( \""<< state_id <<"\" );\n";
  if(!parentName.empty())
  {
    fs << "  setParentStateName( \"" << parentName << "\" );\n";
  }
  fs << "}\n\n";

  fs << "State"<< state_id <<"::~State"<< state_id <<"()\n";
  fs << "{\n";
  fs << "}\n\n";



//  {
//    my $enterStateName = YaFsmScxmlParser::getEnterStateName($state);
//    if(( YaFsmScxmlParser::hasStateEnterActions($state) ) || (defined $enterStateName) )
//    {
//      fs << "void State"<< state_id <<"::enter( " << mDataModel["name"] << "& fsmImpl )\n";
//    }
//    else
//    {
//      fs << "void State"<< state_id <<"::enter( " << mDataModel["name"] << "& /*fsmImpl*/ )\n";
//    }
//    fs << "{\n";

//    if(YaFsmScxmlParser::hasStateEnterActions($state))
//    {
//      if(%YaFsmScxmlParser::gFSMDataModel && defined $state->{onentry}{script})
//      {
//        fs << ( "  " << state_id <<"_onEntry(fsmImpl.model());\n" );
//      }

//      if(defined $state->{onentry}{raise})
//      {
//        foreach(@{$state->{onentry}{raise}})
//        {
//          fs << "  fsmImpl.sendEvent(\"$state->{id}\", " . $_->{event}."(), 0);\n";
//        }
//      }
//      if(defined $state->{onentry}{send})
//      {
//        foreach(@{$state->{onentry}{send}})
//        {
//          genSendEventImpl($fhS, $state->{id}, $_);
//        }
//      }
//      if(defined $state->{onentry}{cancel})
//      {
//        foreach(@{$state->{onentry}{cancel}})
//        {
//          genCancelEventImpl($fhS, $_);
//        }
//      }

//    }

//    if( defined $enterStateName )
//    {
//      fs << "  fsmImpl.setStateByName( \"$enterStateName\" );\n";
//      fs << "  fsmImpl.enterCurrentState();\n";
//    }
//    fs << "}\n\n";


//    if(YaFsmScxmlParser::hasStateExitActions($state))
//    {
//      fs << "void State"<< state_id <<"::exit( " << mDataModel["name"] << "& fsmImpl )\n";
//      fs << "{\n";


//      if(%YaFsmScxmlParser::gFSMDataModel && defined $state->{onexit}{script})
//      {
//        fs <<  "  " << state_id << "_onExit(fsmImpl.model());\n" ;
//      }

//      if(defined $state->{onexit}{raise})
//      {
//        foreach(@{$state->{onexit}{raise}})
//        {
//          fs << "  fsmImpl.sendEvent( \"$state->{id}\", ". $_->{event} . "(), 0);\n";
//        }
//      }

//      if(defined $state->{onexit}{send})
//      {
//        foreach(@{$state->{onexit}{send}})
//        {
//          genSendEventImpl($fhS, $state->{id}, $_);
//        }
//      }
//      if(defined $state->{onexit}{cancel})
//      {
//        foreach(@{$state->{onexit}{cancel}})
//        {
//          genCancelEventImpl($fhS, $_);
//        }
//      }


//    }
//    else
//    {
//      fs << "void State"<< state_id <<"::exit( " << mDataModel["name"] << "& /*fsmImpl*/ )\n";
//      fs << "{\n";
//    }
//    fs << "}\n\n";


//  }

}
