#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#define TESTFSM
#include "EventScxmlFSM.h"
#include <thread>
#include <chrono>

TEST_CASE( "init fsm" )
{
  EventScxmlFSM EventFSM;
  EventFSM.initFSM();
  REQUIRE( EventFSM.model().mTestState == "EnterStop" );
  EventFSM.sendEvent( AutoStart() );
  std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
  REQUIRE( EventFSM.model().mEnterTestString == "TriggerAutoStart;ExitStop;AutoStart;EnterRun" );
  REQUIRE( EventFSM.model().mExitTestString == "ExitRun;AutoEnd;EnterFinal" );

  EventFSM.dumpCoverage();
}
