#pragma once

#include <string>

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

class EventScxmlFSMDataModel
{
public:
  EventScxmlFSMDataModel()
    : mTestState( "" )
  {
  }
  virtual ~EventScxmlFSMDataModel() = default;

  void enterRun( const std::string& str ) { mEnterTestString = str; };
  void exitRun( const std::string& str ) { mExitTestString = str; };

public:
  virtual void onEnterStop( void ) { mTestState = "EnterStop"; }
  virtual void onTriggerAutoStart( void ) { mTestState = "TriggerAutoStart"; }

  virtual void onExitStop( void ) { mTestState += ";ExitStop"; }
  virtual void onAutoStart( void ) { mTestState += ";AutoStart"; }
  virtual void onEnterRun( void )
  {
    mTestState += ";EnterRun";
    enterRun( mTestState );
    mTestState.clear();
  }
  virtual void onExitRun( void ) { mTestState += "ExitRun"; }
  virtual void onAutoEnd( void ) { mTestState += ";AutoEnd"; }
  virtual void onEnterFinal( void )
  {
    mTestState += ";EnterFinal";
    exitRun( mTestState );
    mTestState.clear();
  }

public:
  std::string mTestState       = {};
  std::string mEnterTestString = {};
  std::string mExitTestString  = {};
};
