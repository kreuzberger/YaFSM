#ifndef IFSMEVENTCB_H
#define IFSMEVENTCB_H

class IFSMEventCB
{
  public:
  IFSMEventCB(){};
  virtual ~IFSMEventCB(){};

  public:
  virtual void processEventID(int iEventID) = 0 ;
};


#endif
 
