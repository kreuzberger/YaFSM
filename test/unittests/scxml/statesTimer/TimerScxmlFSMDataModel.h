#ifndef TIMERSCXMLFSMDATAMODEL_H
#define TIMERSCXMLFSMDATAMODEL_H


#include <QtCore/QObject>
#include <QtCore/QTime>

class AutoEnd
{
};

class AutoStart
{
public:
  int i;

};

class run
{
public:
  run() : mCarType(0) {}
  int data() const { return mCarType; }
  int mCarType;

};

class TimerScxmlFSMDataModel: public QObject
{
  Q_OBJECT
public:
  TimerScxmlFSMDataModel();
  virtual ~TimerScxmlFSMDataModel();

signals:
  void enterRun(QTime);
  void exitRun( QTime);

public:
  virtual void onEnterStop( void )   { mTestTime = QTime::currentTime(); }
  virtual void onTriggerAutoStart( void )  { mTestTime = QTime::currentTime(); }

  virtual void onExitStop( void )   { mTestTime = QTime::currentTime(); }
  virtual void onAutoStart( void ) { mTestTime = QTime::currentTime();}
  virtual void onEnterRun( void )   { mTestTime = QTime::currentTime(); emit enterRun(mTestTime); }
  virtual void onExitRun( void )   { mTestTime = QTime::currentTime(); }
  virtual void onAutoEnd( void )   { mTestTime = QTime::currentTime(); }
  virtual void onEnterFinal( void )   { mTestTime = QTime::currentTime();; emit exitRun(mTestTime);  }

public:
  QTime mTestTime;
};


#endif
