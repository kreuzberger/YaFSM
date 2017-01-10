#ifndef ISCXMLFSMEVENTCB_H
#define ISCXMLFSMEVENTCB_H

class IScxmlFSMEventCB
{
  public:
  IScxmlFSMEventCB();
  virtual ~IScxmlFSMEventCB();
  public:
  virtual void processTimerEventID(int iEventID, int id) = 0 ;
};


#endif
 
