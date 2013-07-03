
#include "TestFullFSM.h"
#include <QtTest/QTest>

QTEST_MAIN(QTestFullFSM)

void QTestFullFSM::testInitFSM()
{
  QString state;
  
  state = mFullFSM.getStateName().c_str();
  
  QCOMPARE(QString("Statestop"),state);

  mFullFSM.run(true, true);

  state = mFullFSM.getStateName().c_str();
  QCOMPARE(QString("Statedeep3"),state);

  mFullFSM.end();

 state = mFullFSM.getStateName().c_str();
  QCOMPARE(QString("Statestop"),state);

  mFullFSM.dumpCoverage();

}
