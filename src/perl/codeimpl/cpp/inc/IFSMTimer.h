#ifndef IFSMTIMER_H
#define IFSMTIMER_H

class IFSMTimer
{
  public:
  IFSMTimer(){};
  virtual ~IFSMTimer(){};

  public:
  virtual void setTimerID( int iTimerId, int iTimeOutMs, int iRepeat) = 0;
  virtual void startTimerID( int iTimerId ) = 0;
  virtual void stopTimerID( int iTimerId ) = 0;

};


#endif
 
