#pragma once

class IScxmlFSMEventCB
{
  public:
  IScxmlFSMEventCB();
  virtual ~IScxmlFSMEventCB();
  public:
  virtual void processTimerEventID(int iEventID, int id) = 0 ;
};

 
