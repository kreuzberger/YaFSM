
#include "TestTriggerFSM.h"

#include <QtTest/QTest>

QTEST_MAIN(TestTriggerFSM)

void TestTriggerFSM::testInitFSM()
{
  QString str;
  str = mTriggerFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  mTriggerFSM.run();
  str = mTriggerFSM.getStateName().c_str();
  // first state under state run
  QCOMPARE(QString("Statestopping"),str);

  mTriggerFSM.run();
  str = mTriggerFSM.getStateName().c_str();
  QCOMPARE(QString("Staterunning"),str);

  mTriggerFSM.end();
  str = mTriggerFSM.getStateName().c_str();
  QCOMPARE(QString("Statestopping"),str);
  str = mTriggerFSM.getStateName().c_str();
  mTriggerFSM.end();
  str = mTriggerFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  mTriggerFSM.dumpCoverage();
}
