
#include "TestTransScxmlFSM.h"
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

QTEST_MAIN(QTestTransScxmlFSM)

void QTestTransScxmlFSM::testInitFSM()
{



  QSignalSpy spyEnterRunning(&mTransScxmlFSM.model(), SIGNAL(enterRunning(QString)));
  QSignalSpy spyEnterFinal(&mTransScxmlFSM.model(), SIGNAL(enterFinal(QString)));

  QString str;
  str = mTransScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("stop"),str);

  run a;
  a.mData.iValid = true;
  mTransScxmlFSM.sendEvent(a);
  str = mTransScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("running"),str);

  QCOMPARE(spyEnterRunning.count(), 1); // make sure the signal was emitted exactly one time
  QList<QVariant> args = spyEnterRunning.takeFirst();
  QCOMPARE(args.length(), 1);
  QCOMPARE(args.at(0).toString(),QString("onEnterStop;onRun;onEnterRun;onEnterRunning"));


  mTransScxmlFSM.sendEvent(end());
  str = mTransScxmlFSM.getStateName().c_str();
  QCOMPARE(QString("FinalState"),str);

  QCOMPARE(spyEnterFinal.count(), 1); // make sure the signal was emitted exactly one time
  args = spyEnterFinal.takeFirst();
  QCOMPARE(args.length(), 1);
  QCOMPARE(args.at(0).toString(),QString("onExitRunning;onExitRun;onEnd;onEnterFinal"));

  mTransScxmlFSM.dumpCoverage();
}
