#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#define TESTFSM
#include "TimerScxmlFSM.h"
#include <thread>
#include <chrono>

TEST_CASE( "init fsm" )
{
  TimerScxmlFSM TimerFSM;
  TimerFSM.initFSM();
  auto starttime = std::chrono::steady_clock::now();
  TimerFSM.sendEvent( AutoStart() );

  std::this_thread::sleep_for( std::chrono::milliseconds( 4000 ) );

  auto milliSecsTo = std::chrono::duration_cast<std::chrono::milliseconds>( TimerFSM.model().mEnterTime - starttime ).count();
  // should be on second, but timer can have also negative latency
  REQUIRE( milliSecsTo >= 900 );

  milliSecsTo = std::chrono::duration_cast<std::chrono::milliseconds>( TimerFSM.model().mExitTime - starttime ).count();
  // should be tow seconds, but timer can have also negative latency
  REQUIRE( milliSecsTo >= 1900 );

  TimerFSM.dumpCoverage();
}
