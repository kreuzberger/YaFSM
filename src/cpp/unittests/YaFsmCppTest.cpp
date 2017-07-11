#include "YaFsmCppTest.h"
#include "src/cpp/YaFsmScxmlParser.h"
#include <iostream>

YaFsmCppTest::YaFsmCppTest()
{
}


void YaFsmCppTest::runtests()
{
  testDataModel();
}

void YaFsmCppTest::test( const std::string& actual, const std::string expected)
{
  if( actual != expected)
  {
    std::cerr << actual << " does not match " << expected << std::endl;
    exit(1);
  }
}


void YaFsmCppTest::testDataModel()
{
  std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<scxml xmlns=\"http://www.w3.org/2005/07/scxml\" version=\"1.0\" binding=\"early\" xmlns:qt=\"http://www.qt.io/2015/02/scxml-ext\" name=\"SimpleScxmlFSM\" qt:editorversion=\"4.2.0\" datamodel=\"cplusplus:SimpleScxmlFSMDataModel:SimpleScxmlDataModel.h\" initial=\"stop\">\n</scxml>";
  YaFsmScxmlParser parser;
  tinyxml2::XMLDocument doc;
  doc.Parse(xml.c_str());
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if(elem)
  {
    parser.parseDefinitions(elem);
  }

  test(parser.mDataModel["headerfile"],"SimpleScxmlDataModel.h");
}
