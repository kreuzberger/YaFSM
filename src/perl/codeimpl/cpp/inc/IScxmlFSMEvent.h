#ifndef ISCXMLFSMEVENT_H
#define ISCXMLFSMEVENT_H

class IScxmlFSMEvent
{
  public:
  IScxmlFSMEvent();
  virtual ~IScxmlFSMEvent();

  public:
  virtual void setEventID( int eventID) = 0;
  virtual int sendEventID( int eventID, int delayMs ) = 0;
  virtual void cancelEvent( int sendID ) = 0;

};


#endif
 
