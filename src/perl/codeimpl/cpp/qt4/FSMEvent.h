#ifndef FSMEVENT_H
#define FSMEVENT_H

#include "IFSMEvent.h"
#include "IFSMEventCB.h"

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QMap>

class FSMEvent: public QObject, public IFSMEvent
{
Q_OBJECT
  public:
    FSMEvent(IFSMEventCB&);
    virtual ~FSMEvent();
    
    virtual void registerEventID( int );
    virtual void sendEventID( int );
    virtual bool event( QEvent* e );
    
  private:
    FSMEvent();
    FSMEvent& operator=(const FSMEvent&);
    FSMEvent(const FSMEvent&);
    
    IFSMEventCB& mCBHandler;
    QMap<int,QEvent::Type> mEventMap;
    
    


};


#endif


