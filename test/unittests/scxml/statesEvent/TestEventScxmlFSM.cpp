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
#include "EventScxmlFSM.h"
#include <QtTest/QtTest>
#include <thread>
#include <chrono>

TEST_CASE( "init fsm" )
{
  EventScxmlFSM EventFSM;
  EventFSM.initFSM();
  REQUIRE( EventFSM.model().mTestState == "EnterStop" );
  EventFSM.sendEvent( AutoStart() );
  QTest::qWait( 500 );
  REQUIRE( EventFSM.model().mEnterTestString == "TriggerAutoStart;ExitStop;AutoStart;EnterRun" );
  REQUIRE( EventFSM.model().mExitTestString == "ExitRun;AutoEnd;EnterFinal" );

  EventFSM.dumpCoverage();
}
