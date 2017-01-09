#ifndef ISCXMLFSMEVENT_H
#define ISCXMLFSMEVENT_H

class IScxmlFSMEvent
{
  public:
  IScxmlFSMEvent();
  virtual ~IScxmlFSMEvent();

  public:
  virtual void setEventID( int eventID) = 0;
  virtual void sendEventID( int eventID, int delayMs ) = 0;
  virtual void cancelEventID( int eventID ) = 0;

};


#endif
 
