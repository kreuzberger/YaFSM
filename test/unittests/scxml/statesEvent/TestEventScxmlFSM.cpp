
#include "TestEventScxmlFSM.h"
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

QTEST_MAIN(QTestEventScxmlFSM)

void QTestEventScxmlFSM::testInitFSM()
{

  //std::string str = mpApp->mEventFSM.getStateName();
  QSignalSpy spyEnterRun(&mpApp->mEventFSM.model(), SIGNAL(enterRun(QString)));
  QSignalSpy spyExitRun(&mpApp->mEventFSM.model(), SIGNAL(exitRun(QString)));
  QCOMPARE(mpApp->mEventFSM.model().mTestState,QString("EnterStop"));
  mpApp->mEventFSM.sendEvent(AutoStart());
  qApp->processEvents();
  qApp->processEvents();
  QTest::qWait(100); // wait for events to be delivered
  QCOMPARE(spyEnterRun.count(), 1); // make sure the signal was emitted exactly one time
  QList<QVariant> args = spyEnterRun.takeFirst();
  QCOMPARE(args.length(), 1);
  QCOMPARE(args.at(0).toString(),QString("TriggerAutoStart;ExitStop;AutoStart;EnterRun"));

  QCOMPARE(spyExitRun.count(), 1); // make sure the signal was emitted exactly one time
  args = spyExitRun.takeFirst();
  QCOMPARE(args.length(), 1);
  QCOMPARE(args.at(0).toString(),QString("ExitRun;AutoEnd;EnterFinal"));


//  QCOMPARE(mpApp->mEventFSM.model().mTestState,std::string("Statefinished"));

  mpApp->mEventFSM.dumpCoverage();
}

