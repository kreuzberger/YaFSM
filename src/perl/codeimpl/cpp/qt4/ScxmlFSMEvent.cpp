#include "ScxmlFSMEvent.h"
#include <QtCore/QTimer>
#include <QtCore/QTimerEvent>
#include <QtCore/QObject>
#include <assert.h>

IScxmlFSMEvent::IScxmlFSMEvent()
{
}

IScxmlFSMEvent::~IScxmlFSMEvent()
{
}

IScxmlFSMEventCB::IScxmlFSMEventCB()
{
}

IScxmlFSMEventCB::~IScxmlFSMEventCB()
{
}


ScxmlFSMEvent::ScxmlFSMEvent( IScxmlFSMEventCB& handler)
: QObject()
, IScxmlFSMEvent()
, mCbHandler( handler )
//, mEventMap()
, mActiveEventMap()
{

}

ScxmlFSMEvent::~ScxmlFSMEvent()
{


}

void ScxmlFSMEvent::setEventID( int eventID)
{
  if(!mEventMap.contains(eventID))
  {
    mEventMap.insert(eventID, 0);
  }
  
}


int ScxmlFSMEvent::sendEventID( int eventID, int delayMs )
{
  int id = 0;
  if(mEventMap.contains(eventID))
  {
    int iActiveTimerID = startTimer(delayMs);
    mActiveEventMap.insert(eventID,iActiveTimerID);
    id = iActiveTimerID;
  }
  assert( 0 < id );
  return id;
}

void ScxmlFSMEvent::cancelEvent( int sendID )
{
  if(mActiveEventMap.values().contains(sendID))
  {
    killTimer(sendID);
    int eventID = mActiveEventMap.key(sendID);
    mActiveEventMap.remove(eventID);
  }
} 

void ScxmlFSMEvent::timerEvent(QTimerEvent* pEvent)
{
  if( 0 != pEvent)
  {
    if(mActiveEventMap.values().contains(pEvent->timerId()))
    {
      int eventID = mActiveEventMap.key(pEvent->timerId());
      if (0 != eventID )
      {
        mCbHandler.processTimerEventID(eventID, mActiveEventMap[eventID]);
        cancelEvent(mActiveEventMap[eventID]);
      }
    }
  }
}

