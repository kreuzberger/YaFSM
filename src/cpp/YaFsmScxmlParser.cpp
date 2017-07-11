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


  //    my $fsmbasename=basename($fsmname,'.scxml');
//    if((!defined $gFSMCodeOutPath))
//    {
//      $gFSMCodeOutPath= cwd() . '/' .lc($fsmbasename).'/code';
//    }

//    if($gFSMGenCode && (defined $gFSMCodeOutPath))
//    {
//      mkpath( $gFSMCodeOutPath, {verbose => 1, mode => 0755}) if (!(-d $gFSMCodeOutPath));
//    }
//  }
}

void YaFsmScxmlParser::readFSM()
{

  tinyxml2::XMLDocument doc;
  doc.LoadFile( mFileName.c_str() );
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if(elem)
  {
    parseDefinitions(elem);
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

//      YaFsm::printDbg("data model string $root->getAttribute('datamodel')") if( $strDataModel );
//      my @modelInfo = split(/:/,$strDataModel);
//      YaFsm::printDbg(@modelInfo);

//      if($#modelInfo == 2 )
//      {
//        if ("cplusplus" eq $modelInfo[0])
//        {
//          $gFSMDataModel{type}=$modelInfo[0];
//          $gFSMDataModel{classname}=$modelInfo[1];
//          $gFSMDataModel{headerfile}=$modelInfo[2];
//        }
//        else
//        {
//          YaFsm::printFatal("invalid datamodel type $modelInfo[0]");
//        }

//      }
//      else
//      {
//        YaFsm::printFatal("invalid datamodel definition string $strDataModel");
//      }
//    }

//    # check for data members

//    foreach my $data ($currRef->findnodes('/datamodel/data'))
//    {
//      #YaFsm::printDbg("data:  $data->{id} $data->{expr}");
//      if ( $data->{expr} )
//      {
//        $gFSMMembers{$data->{id}}= { expr => $data->{expr}} ;
//      }
//      elsif ($data->{src})
//      {
//        my @memberInfo = split(/:/,$data->{src});

//        if($#memberInfo == 1 )
//        {
//          $gFSMMembers{$data->{id}}= { classname => $memberInfo[0], src => $memberInfo[1] } ;
//        }
//      }
//    }
//  }
}
