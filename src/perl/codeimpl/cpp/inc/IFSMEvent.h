#ifndef IFSMEVENT_H
#define IFSMEVENT_H

class IFSMEvent
{
  public:
    IFSMEvent() {}
    virtual ~IFSMEvent() {}
    
    virtual void registerEventID( int ) = 0;
    virtual void sendEventID( int ) = 0;
  

};


#endif


