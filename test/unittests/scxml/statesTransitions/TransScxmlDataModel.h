#ifndef TRANSSCXMLFSMDATAMODEL_H
#define TRANSSCXMLFSMDATAMODEL_H

#include <QtCore/QString>
#include <QtCore/QObject>


class run
{
public:
  run() : mData() {}
  class Data
  {
  public:
    Data():iValid(false) {}
    bool iValid;
  };

public:
   Data data() const { return mData; }

public:
   Data mData;
};

class end
{
};

class TransScxmlFSMDataModel: public QObject
{
  Q_OBJECT
public:
  TransScxmlFSMDataModel()
  : mTestState("")
  {}

signals:
  void enterRunning(QString);
  void enterFinal( QString);

public:
  void onEnterStop()    { mTestState = "onEnterStop"; }
  void onRun()          { mTestState += ";onRun"; }
  void onRunInvalid()   { mTestState += ";onRunInvalid"; }
  void onEnterRun()     { mTestState += ";onEnterRun";}
  void onEnterRunning() { mTestState += ";onEnterRunning"; emit enterRunning(mTestState); mTestState.clear(); }

  void onExitRunning()  { mTestState = "onExitRunning"; }
  void onExitRun()      { mTestState += ";onExitRun"; }
  void onEnd()          { mTestState += ";onEnd"; }
  void onEnterFinal()   { mTestState += ";onEnterFinal"; emit enterFinal(mTestState); mTestState.clear(); }


private:
  QString mTestState;

};


#endif
