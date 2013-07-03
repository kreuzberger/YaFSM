#ifndef FSMEVENT_H
#define FSMEVENT_H

#include "IFSMEvent.h"
#include "IFSMEventCB.h"

class FSMEvent: public QObject, public IFSMEvent
{
Q_OBJECT
  public:
    FSMEvent(IFSMEventCB&);
    virtual ~FSMEvent();


  private:
    FSMEvent();
    FSMEvent& operator=(const FSMEvent&);
    FSMEvent(const FSMEvent&);

    virtual void registerEventID( int );
    virtual void sendEventID( int );

    IFSMEventCB& mCBHandler;
};


#endif


