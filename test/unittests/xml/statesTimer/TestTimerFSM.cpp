
#include "TestTimerFSM.h"
#include <QtTest/QTest>

QTEST_MAIN(TestTimerFSM)


void TestTimerFSM::testInitFSM()
{
  QString str = mTimerFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  QTest::qWait(8000);

  str =  mTimerFSM.getStateName().c_str();
  QCOMPARE(QString("Statefinish"),str);

  mTimerFSM.dumpCoverage();
}

