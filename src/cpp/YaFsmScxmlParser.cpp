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

std::string YaFsmScxmlParser::getEnterStateName(const tinyxml2::XMLElement * elem )
{
  std::string name;
  const tinyxml2::XMLElement* initial = elem->FirstChildElement("inital");
  if(initial)
  {
    const tinyxml2::XMLElement* transition = elem->FirstChildElement("transition");
    if(transition)
    {
      name = transition->Attribute("target");
    }
  }

  return name;
}

bool YaFsmScxmlParser::hasStateActions(const std::string& str, const tinyxml2::XMLElement * elem)
{
  bool has = false;
  const tinyxml2::XMLElement* action = elem->FirstChildElement(str.c_str());
  if( action )
  {
    has = true;
  }

  return has;
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

    genTransImpl(fh, fs, state, parentName );

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

void YaFsmScxmlParser::genStateActions(std::ofstream& fs, const std::string& state_id, const tinyxml2::XMLElement* elem)
{

  const tinyxml2::XMLElement* action = elem->FirstChildElement();
  while(action)
  {
    if(std::string("script") == std::string(action->Name()))
    {
      if(mVerbose) std::cout << "implement script action for state " << state_id << std::endl;
      std::string text = action->GetText();
      fs << "  " << text << "\n";
    }
    else if( std::string("raise") == std::string(action->Name()))
    {
      if(mVerbose) std::cout << "implement raise action for state " << state_id << std::endl;
      fs << "  fsmImpl.sendEvent(\"" << state_id << "\", "  << action->Attribute("event") <<"(), 0);\n";
    }
    else if( std::string("send") == std::string(action->Name()))
    {
      if(mVerbose) std::cout << "implement send action for state " << state_id << std::endl;
      std::string event = action->Attribute("event");
      fs <<  "  " << event << " data" << event << ";\n";
      const tinyxml2::XMLElement* para = action->FirstChildElement("param");
      if( para )
      {
        if(para->Attribute("expr"))
        {
          fs << "  data" << event << "." << para->Attribute("name") << " = " << para->Attribute("expr")  <<";\n";
        }
        para = para->NextSiblingElement("para");
      }


      if(action->Attribute("id"))
      {
        YaFsm::printWarn("use of generated ids for send event not allowed!\n!");
      }
      else if(action->Attribute("idlocation"))
      {
        YaFsm::printWarn("use of idlocation for send event not allowed!\n!");
      }
      fs << "  " << "fsmImpl.sendEvent( \"" << state_id << "\", data"<< event << ", $eventPara->{delay});\n";

    }
    else if( std::string("cancel") == std::string(action->Name()) )
    {
      if(mVerbose) std::cout << "implement cancel action for state " << state_id << std::endl;

      fs << "  fsmImpl.cancelEvent(\"" << state_id << "\", "  << action->Attribute("sendid") <<"(), 0);\n";
    }

    action = action->NextSiblingElement();
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
      std::string text = script->GetText();
      fs << "  " << text << "\n";
      script = script->NextSiblingElement("script");
    }

    fs << "}\n\n";

    transIdx++;

    transition = transition->NextSiblingElement("transition");

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


  std::string enterStateName = getEnterStateName(state);
  if( !enterStateName.empty() || hasStateActions("onentry",state))
  {
    fs << "void State"<< state_id <<"::enter( " << mDataModel["name"] << "& fsmImpl )\n";
  }
  else
  {
    fs << "void State"<< state_id <<"::enter( " << mDataModel["name"] << "& /*fsmImpl*/ )\n";
  }

  fs << "{\n";

  if( hasStateActions("onentry",state))
  {
    fs << "  model = fsmImpl.model();\n";

    genStateActions(fs, state_id, state->FirstChildElement("onentry"));


    if( !enterStateName.empty() )
    {
      fs << "  fsmImpl.setStateByName( \"" << enterStateName <<"\" );\n";
      fs << "  fsmImpl.enterCurrentState();\n";
    }
  }

  fs << "}\n\n";

  if( hasStateActions("onexit",state))
  {
    fs << "void State"<< state_id <<"::exit( " << mDataModel["name"] << "& fsmImpl )\n";
  }
  else
  {
    fs << "void State"<< state_id <<"::exit( " << mDataModel["name"] << "& /*fsmImpl*/ )\n";
  }

  fs << "{\n";

  if( hasStateActions("onexit",state))
  {
    fs << "  model = fsmImpl.model();\n";

    genStateActions(fs, state_id, state->FirstChildElement("onexit"));
  }

  fs << "}\n\n";
}

void YaFsmScxmlParser::genTransImpl(std::ofstream &fh, std::ofstream &fs, const tinyxml2::XMLElement *state, const std::string& parentName)
{

  std::map<std::string, bool> processedTriggers;

  const tinyxml2::XMLElement* transition = state->FirstChildElement("transition");
  while( transition)
  {
    std::string event = std::string(transition->Attribute("event"));
///    std::string source = std::string(transition->Attribute("source"));
    std::string source = std::string(state->Attribute("id"));
    std::string target = source;
    if(state->Attribute("target"))
    {
      target = state->Attribute("target");
    }
    std::string criteria = source + "_" + event;
    //    # add methods for each trigger.
    //    # be carefull that a trigger is not implemented twice, depending on conditions
    //    # so consider trigger name and begin of a trigger as condition if already implemented
    auto it = processedTriggers.find(criteria);
    if(!event.empty() && it == processedTriggers.end())
    {
      processedTriggers[criteria] = true;
      fs << "void State" << source << "::send_" << event << "(" << mDataModel["name"] << "& fsmImpl, const " << event << "& _event)\n";
      fs << "{\n";

//      push(@{$genTransitions},$transCoverageName);

      fs << "  (void) fsmImpl;\n";

//      for(my $nextIdx=$idx; $nextIdx < @transArray; $nextIdx++)
//      {
//        if($transArray[$nextIdx]->{event} eq $trans->{event})
//        {
//          if($transArray[$nextIdx]->{source} eq $trans->{source})
//          {
//            if($transArray[$nextIdx]->{cond})
//            {
//              print $fhS "  if ($transArray[$nextIdx]->{cond})\n"; #todo implement conditions
//            }
//            print $fhS "  {\n";
//            print $fhS "    fsmImpl.setTransByName(\"$transCoverageName\");\n";
//            # current could be difficult to determine. if we made a fallthrough into next hierarchy level
//            # exit state by name
//            if( $transArray[$nextIdx]->{source} ne $transArray[$nextIdx]->{target} )
//            {
//              print $fhS "    fsmImpl.exitState(\"" . $transArray[$nextIdx]->{source} ."\");\n" ;
//            }

//            if(YaFsmScxmlParser::hasTransitionActions($transArray[$nextIdx]))
//            {
//              print $fhS ( "    transition_" . $transArray[$nextIdx]->{source} . "_" . $transArray[$nextIdx]->{event} . "_$idx(fsmImpl.model(), _event);\n" );
//            }
//            if(YaFsmScxmlParser::hasTransitionEvents($transArray[$nextIdx]))
//            {
//              foreach(@{$transArray[$nextIdx]->{raise}})
//              {
//                print $fhS "    fsmImpl.sendEvent( \"$transArray[$nextIdx]->{event}\", " . $_->{event} . "(), 0);\n";
//              }
//              foreach(@{$transArray[$nextIdx]->{send}})
//              {
//                genSendEventImpl($fhS, $transArray[$nextIdx]->{event}, $_);
//              }
//              foreach(@{$transArray[$nextIdx]->{cancel}})
//              {
//                genCancelEventImpl($fhS, $_);
//              }
//            }

//            if( $transArray[$nextIdx]->{source} ne $transArray[$nextIdx]->{target} )
//            {
//              print $fhS '    fsmImpl.setStateByName("' . $transArray[$nextIdx]->{target} . "\");\n";
//              print $fhS "    fsmImpl.enterCurrentState();\n";
//            }

//            if($transArray[$nextIdx]->{cond} && (defined $parentName) && (0 < length($parentName)))
//            {
//              print $fhS "  }\n";
//              print $fhS "  else // condition is not matched, we should now try if condition is matched by parent\n";
//              print $fhS "  {\n";

//              print $fhS "    State$parentName" . "::send_$trans->{event}( fsmImpl, $trans->{event} );\n";
//            }

//            print $fhS "  }\n\n";
//          }
//        }
//      }
      fs << "}\n\n";
    }
//    $idx++;
    transition = transition->NextSiblingElement("transition");
  }

}
