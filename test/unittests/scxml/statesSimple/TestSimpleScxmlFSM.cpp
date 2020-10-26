#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

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
