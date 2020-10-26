#pragma once

#include <thread>
#include <chrono>

class AutoEnd
{
};

class AutoStart
{
public:
  int i;
};

class Cancel
{
};

class run
{
public:
  run()
    : mCarType( 0 )
  {
  }
  int data() const { return mCarType; }
  int mCarType;
};

class TimerScxmlFSMDataModel
{
public:
  TimerScxmlFSMDataModel()          = default;
  virtual ~TimerScxmlFSMDataModel() = default;

public:
  virtual void onEnterStop( void ) { mTestTime = std::chrono::steady_clock::now(); }
  virtual void onTriggerAutoStart( void ) { mTestTime = std::chrono::steady_clock::now(); }

  virtual void onExitStop( void ) { mTestTime = std::chrono::steady_clock::now(); }
  virtual void onAutoStart( void ) { mTestTime = std::chrono::steady_clock::now(); }
  virtual void onEnterRun( void )
  {
    mTestTime  = std::chrono::steady_clock::now();
    mEnterTime = mTestTime;
  }
  virtual void onExitRun( void ) { mTestTime = std::chrono::steady_clock::now(); }
  virtual void onAutoEnd( void ) { mTestTime = std::chrono::steady_clock::now(); }
  virtual void onEnterFinal( void )
  {
    mTestTime = std::chrono::steady_clock::now();
    mExitTime = mTestTime;
  }

public:
  std::chrono::time_point<std::chrono::steady_clock> mTestTime;
  std::chrono::time_point<std::chrono::steady_clock> mEnterTime;
  std::chrono::time_point<std::chrono::steady_clock> mExitTime;
};
