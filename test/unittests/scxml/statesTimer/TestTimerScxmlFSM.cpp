
#include "TestTimerScxmlFSM.h"
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

QTEST_MAIN(QTestTimerScxmlFSM)

void QTestTimerScxmlFSM::testInitFSM()
{

  //std::string str = mpApp->mTimerFSM.getStateName();
  QSignalSpy spyEnterRun(&mpApp->mTimerFSM.model(), SIGNAL(enterRun(QTime)));
  QSignalSpy spyExitRun(&mpApp->mTimerFSM.model(), SIGNAL(exitRun(QTime)));
  QTime starttime = QTime::currentTime();
  mpApp->mTimerFSM.sendEvent(AutoStart());
  qApp->processEvents();
  qApp->processEvents();

  QTest::qWait(4000); // wait for events to be delivered


  QCOMPARE(spyEnterRun.count(), 1); // make sure the signal was emitted exactly one time
  QList<QVariant> args = spyEnterRun.takeFirst();
  QCOMPARE(args.length(), 1);
  QTime runEnterTime = args.at(0).toTime();
  int milliSecsTo = starttime.msecsTo(runEnterTime);
  QVERIFY( milliSecsTo >= 1000);

  QCOMPARE(spyExitRun.count(), 1); // make sure the signal was emitted exactly one time
  args = spyExitRun.takeFirst();
  QCOMPARE(args.length(), 1);
  QTime runExitTime = args.at(0).toTime();
  milliSecsTo = runEnterTime.msecsTo(runExitTime);
  QVERIFY( milliSecsTo >= 2000);



//  QCOMPARE(mpApp->mTimerFSM.model().mTestState,std::string("Statefinished"));

  mpApp->mTimerFSM.dumpCoverage();
}

