#pragma once

#include <string>
class run
{

public:
};

class end
{
};

class self_runner
{
};

class SimpleScxmlFSMDataModel
{
public:
  SimpleScxmlFSMDataModel()
    : mTestState( "" )
  {
  }

  // signals:
  void               enterRunning( const std::string& str ) { mCurrentTestString = str; }
  void               enterFinal( const std::string& str ) { mCurrentTestString = str; }
  const std::string& currentTestString() const { return mCurrentTestString; }

public:
  void onEnterStop() { mTestState = "onEnterStop"; }
  void onRun() { mTestState += ";onRun"; }
  void onEnterRun() { mTestState += ";onEnterRun"; }
  void onEnterRunning()
  {
    mTestState += ";onEnterRunning";
    enterRunning( mTestState );
    mTestState.clear();
  }

  void onExitRunning() { mTestState = "onExitRunning"; }
  void onExitRun() { mTestState += ";onExitRun"; }
  void onEnd() { mTestState += ";onEnd"; }
  void onEnterFinal()
  {
    mTestState += ";onEnterFinal";
    enterFinal( mTestState );
    mTestState.clear();
  }

private:
  std::string mTestState         = {};
  std::string mCurrentTestString = {};
};
