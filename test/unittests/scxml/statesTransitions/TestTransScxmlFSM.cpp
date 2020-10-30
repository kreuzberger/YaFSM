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
#include "TransScxmlFSM.h"

TEST_CASE( "init fsm" )
{
  TransScxmlFSM TransScxmlFSM;
  TransScxmlFSM.initFSM();

  REQUIRE( TransScxmlFSM.getStateName() == "stop" );

  run a;
  a.mData.iValid = true;
  TransScxmlFSM.sendEvent( a );
  REQUIRE( TransScxmlFSM.getStateName() == "running" );

  REQUIRE( TransScxmlFSM.model().mRunningTestString == "onEnterStop;onRun;onEnterRun;onEnterRunning" );

  TransScxmlFSM.sendEvent( end() );
  REQUIRE( TransScxmlFSM.getStateName() == "FinalState" );

  REQUIRE( TransScxmlFSM.model().mFinalTestString == "onExitRunning;onExitRun;onEnd;onEnterFinal" );

  TransScxmlFSM.dumpCoverage();
}
