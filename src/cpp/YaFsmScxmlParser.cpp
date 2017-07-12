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

//  my $FSMName = shift;
//  my $outFilePath = shift;
//  #my @actions = shift;
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
//    print $fh "  virtual void send_$key( " . $FSMName . "&, const $key" . "& ) = 0;\n";
  }


  fh << "};\n";
  fh << "\n";
  fh << "#endif\n";
  fh.close();

}


void YaFsmScxmlParser::writeFSMStateBaseHeader()
{
  std::ofstream fh;
  fh.open((mCodeOutDir + YaFsm::sep + std::string("I") + mDataModel["name"] + std::string("StateBase.h")), std::ofstream::out | std::ofstream::trunc);
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
