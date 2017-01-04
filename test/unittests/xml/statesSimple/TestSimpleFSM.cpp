
#include "TestSimpleFSM.h"
#include <QtTest/QTest>

QTEST_MAIN(QTestSimpleFSM)

void QTestSimpleFSM::testInitFSM()
{
  QString str;
  str = mSimpleFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  mSimpleFSM.run();
  str = mSimpleFSM.getStateName().c_str();
  QCOMPARE(QString("Staterunning"),str);

  mSimpleFSM.end();
  str = mSimpleFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  mSimpleFSM.dumpCoverage();
}
