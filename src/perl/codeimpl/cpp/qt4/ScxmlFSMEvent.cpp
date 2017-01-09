#include "ScxmlFSMEvent.h"
#include <QtCore/QTimer>
#include <QtCore/QTimerEvent>
#include <QtCore/QObject>

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
, mEventMap()
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


void ScxmlFSMEvent::sendEventID( int eventID, int delayMs )
{
  if(mEventMap.contains(eventID))
  {
    int iActiveTimerID = startTimer(delayMs);
    mActiveEventMap.insert(eventID,iActiveTimerID);
  }
}

void ScxmlFSMEvent::cancelEventID( int eventID )
{
  if(mActiveEventMap.contains(eventID))
  {
    killTimer(mActiveEventMap[eventID]);
    mActiveEventMap.remove(eventID);

  }
} 

void ScxmlFSMEvent::timerEvent(QTimerEvent* pEvent)
{
  if( 0 != pEvent)
  {
    int eventID = mActiveEventMap.key(pEvent->timerId());
    if (0 != eventID )
    {
      mCbHandler.processTimerEventID(eventID);
      cancelEventID(eventID);
    }
  }
}

