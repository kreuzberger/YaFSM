#define CATCH_CONFIG_RUNNER
#include <QtCore/QCoreApplication>
#include "catch2/catch.hpp" // include after defining CATCH_CONFIG_RUNNER
int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  const int        res = Catch::Session().run( argc, argv );
  return ( res < 0xff ? res : 0xff );
}

#define TESTFSM

#include "SimpleScxmlFSM.h"

TEST_CASE( "init fsm" )
{
  SimpleScxmlFSM mSimpleScxmlFSM;
  mSimpleScxmlFSM.initFSM();

  std::string str;
  str = mSimpleScxmlFSM.getStateName().c_str();
  REQUIRE( str == "stop" );

  mSimpleScxmlFSM.sendEvent( run() );
  str = mSimpleScxmlFSM.getStateName().c_str();
  REQUIRE( str == "running" );

  REQUIRE( mSimpleScxmlFSM.model().currentTestString() == "onEnterStop;onRun;onEnterRun;onEnterRunning" );

  mSimpleScxmlFSM.sendEvent( end() );
  str = mSimpleScxmlFSM.getStateName().c_str();
  REQUIRE( str == "FinalState" );

  REQUIRE( mSimpleScxmlFSM.model().currentTestString() == "onExitRunning;onExitRun;onEnd;onEnterFinal" );

  mSimpleScxmlFSM.dumpCoverage();
}
