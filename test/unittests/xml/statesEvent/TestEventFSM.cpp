
#include "TestEventFSM.h"
#include <QtTest/QTest>

QTEST_MAIN(QTestEventFSM)

void QTestEventFSM::testInitFSM()
{
  QString str = mpApp->mEventFSM.getStateName().c_str();
  QCOMPARE(str,QString("Statestop"));
  mpApp->mEventFSM.sendAutoStart();
  qApp->processEvents();
  QTest::qWait(10); // wait for events to be delivered
  qApp->processEvents();
  str = mpApp->mEventFSM.getStateName().c_str();
  QCOMPARE(str,QString("Statefinished"));

  mpApp->mEventFSM.dumpCoverage();
}

