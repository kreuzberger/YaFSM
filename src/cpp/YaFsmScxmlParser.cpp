#include "YaFsmScxmlParser.h"
#include "YaFsm.h"
#include <iostream>

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
bool YaFsmScxmlParser::genCode() const
{
  return mGenCode;
}

void YaFsmScxmlParser::setGenCode(bool genCode)
{
  mGenCode = genCode;
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
  doc.LoadFile( mFileName.c_str() );
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if(elem)
  {
    parseDefinitions(elem);
    parseFSM(elem);
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
