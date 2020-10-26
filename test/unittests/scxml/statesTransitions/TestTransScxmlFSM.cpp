#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

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
