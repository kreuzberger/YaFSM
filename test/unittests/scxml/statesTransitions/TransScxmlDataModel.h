#pragma once

#include <string>
class run
{
public:
  run()
    : mData()
  {
  }
  class Data
  {
  public:
    Data()
      : iValid( false )
    {
    }
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

class TransScxmlFSMDataModel
{
public:
  TransScxmlFSMDataModel()
    : mTestState( "" )
  {
  }

  void enterRunning( const std::string& str ) { mRunningTestString = str; }
  void enterFinal( const std::string& str ) { mFinalTestString = str; }

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

public:
  std::string mTestState         = {};
  std::string mRunningTestString = {};
  std::string mFinalTestString   = {};
};
