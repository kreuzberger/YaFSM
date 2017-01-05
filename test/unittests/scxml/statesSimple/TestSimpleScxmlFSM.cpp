
#include "TestSimpleScxmlFSM.h"
#include <QtTest/QTest>

QTEST_MAIN(QTestSimpleScxmlFSM)

void QTestSimpleScxmlFSM::testInitFSM()
{
  QString str;
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  run a;
  mSimpleScxmlFSM.sendEvent(a);
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("Staterunning"),str);

  end b;
  mSimpleScxmlFSM.sendEvent(b);
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  mSimpleScxmlFSM.dumpCoverage();
}
