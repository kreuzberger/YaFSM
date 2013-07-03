#ifndef IFSMTIMERCB_H
#define IFSMTIMERCB_H

class IFSMTimerCB
{
  public:
  IFSMTimerCB(){};
  virtual ~IFSMTimerCB(){};

  public:
  virtual void processTimerEventID(int iTimerID) = 0 ;
};


#endif
 
