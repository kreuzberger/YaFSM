#ifndef SIMPLESCXMLFSMDATAMODEL_H
#define SIMPLESCXMLFSMDATAMODEL_H

#include <QtCore/QString>
#include <QtCore/QObject>


class run
{

public:
};

class end
{
};

class SimpleScxmlFSMDataModel: public QObject
{
  Q_OBJECT
public:
  SimpleScxmlFSMDataModel()
  : mTestState("")
  {}

signals:
  void enterRunning(QString);
  void enterFinal( QString);

public:
  void onEnterStop()    { mTestState = "onEnterStop"; }
  void onRun()          { mTestState += ";onRun"; }
  void onEnterRun()     { mTestState += ";onEnterRun";}
  void onEnterRunning() { mTestState += ";onEnterRunning"; emit enterRunning(mTestState); mTestState.clear(); }

  void onExitRunning()  { mTestState = "onExitRunning"; }
  void onExitRun()      { mTestState += ";onExitRun"; }
  void onEnd()          { mTestState += ";onEnd"; }
  void onEnterFinal()   { mTestState += ";onEnterFinal"; emit enterFinal(mTestState); mTestState.clear(); }


private:
  QString mTestState;

};


#endif // SIMPLESCXMLFSMDATAMODEL_H
