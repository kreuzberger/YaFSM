#ifndef EVENTSCXMLFSMDATAMODEL_H
#define EVENTSCXMLFSMDATAMODEL_H


#include <QtCore/QObject>
#include <QtCore/QString>

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

};

class EventScxmlFSMDataModel: public QObject
{
  Q_OBJECT
public:
  EventScxmlFSMDataModel();
  virtual ~EventScxmlFSMDataModel();

signals:
  void enterRun(QString);
  void exitRun( QString);

public:
  virtual void onEnterStop( void )   { mTestState = "EnterStop"; }
  virtual void onTriggerAutoStart( void )  { mTestState = "TriggerAutoStart"; }

  virtual void onExitStop( void )   { mTestState += ";ExitStop"; }
  virtual void onAutoStart( void ) { mTestState += ";AutoStart"; }
  virtual void onEnterRun( void )   { mTestState += ";EnterRun"; emit enterRun(mTestState); mTestState.clear();}
  virtual void onExitRun( void )   { mTestState += "ExitRun"; }
  virtual void onAutoEnd( void )   { mTestState += ";AutoEnd"; }
  virtual void onEnterFinal( void )   { mTestState += ";EnterFinal"; emit exitRun(mTestState); mTestState.clear(); }

public:
  QString mTestState;
};


#endif // SIMPLESCXMLFSMDATAMODEL_H
