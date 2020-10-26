#include "YaFsmCppTest.h"
#include "cpp/YaFsmScxmlParser.h"
#include <iostream>

YaFsmCppTest::YaFsmCppTest()
{
}

void YaFsmCppTest::runtests()
{
  testDataModel();
  testDataModelNode();
  testRootStates();
  testSubStates();
  testEvents();
}

void YaFsmCppTest::test( const std::string& actual, const std::string expected, const std::string& testname )
{
  if ( actual != expected )
  {
    std::cerr << "not ok: " << actual << " does not match " << expected << " " << testname << std::endl;
    exit( 1 );
  }
  else
  {
    std::cout << "ok: " << actual << " matches " << expected << " " << testname << std::endl;
  }
}

void YaFsmCppTest::test( size_t actual, size_t expected, const std::string& testname )
{
  if ( actual != expected )
  {
    std::cerr << "not ok: " << actual << " does not match " << expected << " " << testname << std::endl;
    exit( 1 );
  }
  else
  {
    std::cout << "ok: " << actual << " matches " << expected << " " << testname << std::endl;
  }
}

void YaFsmCppTest::test( bool test, const std::string& testname )
{
  if ( !test )
  {
    std::cerr << "not ok: invalid condition in test " << testname << std::endl;
    exit( 1 );
  }
  else
  {
    std::cout << "ok: " << testname << std::endl;
  }
}

void YaFsmCppTest::testDataModel()
{
  std::string           xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n \
                     <scxml xmlns=\"http://www.w3.org/2005/07/scxml\" version=\"1.0\" binding=\"early\" xmlns:qt=\"http://www.qt.io/2015/02/scxml-ext\" name=\"SimpleScxmlFSM\" qt:editorversion=\"4.2.0\" datamodel=\"cplusplus:SimpleScxmlFSMDataModel:SimpleScxmlDataModel.h\" initial=\"stop\">\n \
                     </scxml>";
  YaFsmScxmlParser      parser;
  tinyxml2::XMLDocument doc;
  doc.Parse( xml.c_str() );
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if ( elem )
  {
    parser.parseDefinitions( elem );
  }

  test( parser.mDataModel["headerfile"], "SimpleScxmlDataModel.h", "Test header file in datamodel" );
}

void YaFsmCppTest::testDataModelNode()
{
  std::string           xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
                     <scxml xmlns=\"http://www.w3.org/2005/07/scxml\" version=\"1.0\" binding=\"early\" xmlns:qt=\"http://www.qt.io/2015/02/scxml-ext\" name=\"SimpleScxmlFSM\" qt:editorversion=\"4.2.0\" datamodel=\"cplusplus:SimpleScxmlFSMDataModel:SimpleScxmlDataModel.h\" initial=\"stop\">\n\
                       <datamodel>\n\
                        <data id=\"timer\" expr=\"2000\"/>\n\
                        <data id=\"other\" src=\"MyTestClass:mTimer\"/>\n\
                       </datamodel>\n\
                     </scxml>";
  YaFsmScxmlParser      parser;
  tinyxml2::XMLDocument doc;
  doc.Parse( xml.c_str() );
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if ( elem )
  {
    parser.parseDefinitions( elem );
  }

  const tinyxml2::XMLElement* dataElem = parser.mMembers["timer"];
  test( dataElem != nullptr, "Test timer data member" );
  const tinyxml2::XMLAttribute* expr = dataElem->FindAttribute( "expr" );
  test( expr->Value(), "2000", "Test timer data value" );
}

void YaFsmCppTest::testRootStates()
{
  std::string           xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
                     <scxml xmlns=\"http://www.w3.org/2005/07/scxml\" version=\"1.0\" binding=\"early\" xmlns:qt=\"http://www.qt.io/2015/02/scxml-ext\" name=\"SimpleScxmlFSM\" qt:editorversion=\"4.2.0\" datamodel=\"cplusplus:SimpleScxmlFSMDataModel:SimpleScxmlDataModel.h\" initial=\"stop\">\n\
                      <state id=\"stop\">\n\
                      </state>\n\
                      <state id=\"run\">\n\
                      </state>\n\
                      <final id=\"FinalState\">\n\
                      </final>\n\
                     </scxml>";
  YaFsmScxmlParser      parser;
  tinyxml2::XMLDocument doc;
  doc.Parse( xml.c_str() );
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if ( elem )
  {
    parser.parseDefinitions( elem );
    parser.parseFSM( elem );
  }

  test( 3 == parser.mStates.size(), "Test root states" );
  const tinyxml2::XMLElement* dataElem = parser.mStates["stop"];
  test( dataElem != nullptr, "Test stop state" );
  dataElem = parser.mStates["run"];
  test( dataElem != nullptr, "Test run state" );
  dataElem = parser.mStates["FinalState"];
  test( dataElem != nullptr, "Test final state" );
}

void YaFsmCppTest::testSubStates()
{
  std::string           xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
                     <scxml xmlns=\"http://www.w3.org/2005/07/scxml\" version=\"1.0\" binding=\"early\" xmlns:qt=\"http://www.qt.io/2015/02/scxml-ext\" name=\"SimpleScxmlFSM\" qt:editorversion=\"4.2.0\" datamodel=\"cplusplus:SimpleScxmlFSMDataModel:SimpleScxmlDataModel.h\" initial=\"stop\">\n\
                      <state id=\"stop\">\n\
                        <state id=\"stop_stop\">\n\
                        </state>\n\
                      </state>\n\
                      <state id=\"run\">\n\
                        <state id=\"run_run\">\n\
                        </state>\n\
                        <final id=\"FinalState\">\n\
                        </final>\n\
                      </state>\n\
                     </scxml>";
  YaFsmScxmlParser      parser;
  tinyxml2::XMLDocument doc;
  doc.Parse( xml.c_str() );
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if ( elem )
  {
    parser.parseDefinitions( elem );
    parser.parseFSM( elem );
  }

  test( 5, parser.mStates.size(), "Test substates" );
  const tinyxml2::XMLElement* dataElem = parser.mStates["stop"];
  test( dataElem != nullptr, "Test stop state" );
  dataElem = parser.mStates["run"];
  test( dataElem != nullptr, "Test run state" );
  dataElem = parser.mStates["run_run"];
  test( dataElem != nullptr, "Test run_run state" );
  dataElem = parser.mStates["stop_stop"];
  test( dataElem != nullptr, "Test stop_stop state" );
  dataElem = parser.mStates["FinalState"];
  test( dataElem != nullptr, "Test final state" );
}

void YaFsmCppTest::testEvents()
{
  std::string           xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n \
                     <scxml xmlns=\"http://www.w3.org/2005/07/scxml\" version=\"1.0\" binding=\"early\" xmlns:qt=\"http://www.qt.io/2015/02/scxml-ext\" name=\"SimpleScxmlFSM\" qt:editorversion=\"4.2.0\" datamodel=\"cplusplus:SimpleScxmlFSMDataModel:SimpleScxmlDataModel.h\" initial=\"stop\">\n \
                     <state id=\"stop\">\n\
                       <state id=\"stop_stop\">\n\
                        <transition type=\"internal\" event=\"end\" target=\"FinalState\">\
                           <raise event=\"event1\"/>\
                           <send event=\"event3\"/>\
                           <raise event=\"event2\"/>\
                        </transition>\
                        <onentry>\
                          <raise event=\"entry\"/>\
                        </onentry>\
                        <onexit>\
                          <send event=\"go1\"/>\
                          <raise event=\"go2\"/>\
                          <send event=\"go3\"/>\
                        </onexit>\
                       </state>\n\
                     </state>\n\
                     </scxml>";
  YaFsmScxmlParser      parser;
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError    error = doc.Parse( xml.c_str() );
  test( error, tinyxml2::XML_SUCCESS, "parsing xml in event test" );
  tinyxml2::XMLElement* elem = doc.FirstChildElement( "scxml" );
  if ( elem )
  {
    parser.parseDefinitions( elem );
    parser.parseFSM( elem );
  }
  test( parser.mTriggers.size(), 1, "Test trigger event parsing in transitions and states" );
  test( parser.mEvents.size(), 7, "Test event parsing in transitions and states" );
}
