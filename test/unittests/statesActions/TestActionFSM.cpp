
#include "TestActionFSM.h"
#include <QtTest/QTest>

QTEST_MAIN(QTestActionFSM)

void QTestActionFSM::testTriggerFSM()
{
  QString state = mActionFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),state);

  mActionFSM.run(false);  //should call onNotRun
  state = mActionFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),state);

  mActionFSM.run(true); // should call onRun, onEnterRun
  state = mActionFSM.getStateName().c_str();
  QCOMPARE(QString("Staterunning"),state);

  mActionFSM.end(); // should call onExitRun
  state = mActionFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),state);

  mActionFSM.run(true); // not triggered any more cause onRun incr variable
  state = mActionFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),state);

  mActionFSM.setStartCounter(0);
  mActionFSM.run(true);
  state = mActionFSM.getStateName().c_str();
  QCOMPARE(QString("Staterunning"),state);

  mActionFSM.dumpCoverage();
}
