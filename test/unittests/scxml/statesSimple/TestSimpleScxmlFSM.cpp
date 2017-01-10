
#include "TestSimpleScxmlFSM.h"
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

QTEST_MAIN(QTestSimpleScxmlFSM)

void QTestSimpleScxmlFSM::testInitFSM()
{



  QSignalSpy spyEnterRunning(&mSimpleScxmlFSM.model(), SIGNAL(enterRunning(QString)));
  QSignalSpy spyEnterFinal(&mSimpleScxmlFSM.model(), SIGNAL(enterFinal(QString)));

  QString str;
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("stop"),str);

  mSimpleScxmlFSM.sendEvent(run());
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("running"),str);

  QCOMPARE(spyEnterRunning.count(), 1); // make sure the signal was emitted exactly one time
  QList<QVariant> args = spyEnterRunning.takeFirst();
  QCOMPARE(args.length(), 1);
  QCOMPARE(args.at(0).toString(),QString("onEnterStop;onRun;onEnterRun;onEnterRunning"));


  mSimpleScxmlFSM.sendEvent(end());
  str = mSimpleScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("FinalState"),str);

  QCOMPARE(spyEnterFinal.count(), 1); // make sure the signal was emitted exactly one time
  args = spyEnterFinal.takeFirst();
  QCOMPARE(args.length(), 1);
  QCOMPARE(args.at(0).toString(),QString("onExitRunning;onExitRun;onEnd;onEnterFinal"));

  mSimpleScxmlFSM.dumpCoverage();
}
