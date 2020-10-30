#define CATCH_CONFIG_RUNNER
#include <QtCore/QCoreApplication>
#include "catch2/catch.hpp" // include after defining CATCH_CONFIG_RUNNER

#include <QtTest/QtTest>

int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  const int        res = Catch::Session().run( argc, argv );
  return ( res < 0xff ? res : 0xff );
}

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

  QTest::qWait( 4000 );

  auto milliSecsTo = std::chrono::duration_cast<std::chrono::milliseconds>( TimerFSM.model().mEnterTime - starttime ).count();
  // should be on second, but timer can have also negative latency
  REQUIRE( milliSecsTo >= 900 );

  milliSecsTo = std::chrono::duration_cast<std::chrono::milliseconds>( TimerFSM.model().mExitTime - starttime ).count();
  // should be tow seconds, but timer can have also negative latency
  REQUIRE( milliSecsTo >= 1900 );

  TimerFSM.dumpCoverage();
}
