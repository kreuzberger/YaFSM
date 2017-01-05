
#include "TestSimpleScxmlFSM.h"
#include <QtTest/QTest>

QTEST_MAIN(QTestSimpleScxmlFSM)

void QTestSimpleScxmlFSM::testInitFSM()
{
  QString str;
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  int i = 3;
  mSimpleScxmlFSM.run(i);
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("Staterunning"),str);

  mSimpleScxmlFSM.end();
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),str);

  mSimpleScxmlFSM.dumpCoverage();
}
