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

ScxmlFSMEvent::ScxmlFSMEvent( IScxmlFSMEventCB& handler )
  : QObject()
  , IScxmlFSMEvent()
  , mCbHandler( handler )
  , mActiveEventMap()
{
}

ScxmlFSMEvent::~ScxmlFSMEvent()
{
}

void ScxmlFSMEvent::setEventID( int eventID )
{
  if ( !mEventMap.contains( eventID ) )
  {
    mEventMap.insert( eventID, 0 );
  }
}

int ScxmlFSMEvent::sendEventID( const std::string& sendId, int eventID, int delayMs )
{
  int id = 0;
  if ( mEventMap.contains( eventID ) )
  {
    int               iActiveTimerID = startTimer( delayMs );
    ScxmlFSMEventInfo eventInfo( eventID, iActiveTimerID );
    mActiveEventMap.insertMulti( sendId, eventInfo );
    id = iActiveTimerID;
  }
  assert( 0 < id );
  return id;
}

std::vector<int> ScxmlFSMEvent::cancelEvent( const std::string& sendId )
{
  std::vector<int> canceledEvents;
  if ( mActiveEventMap.contains( sendId ) )
  {
    auto it = mActiveEventMap.find( sendId );
    while ( it != mActiveEventMap.end() && it.key() == sendId )
    {
      killTimer( it.value().mTimerID );
      canceledEvents.push_back( it.value().mTimerID );
      ++it;
    }
    mActiveEventMap.remove( sendId );
  }
  return canceledEvents;
}

void ScxmlFSMEvent::timerEvent( QTimerEvent* pEvent )
{
  if ( 0 != pEvent )
  {
    auto it = mActiveEventMap.begin();
    while ( it != mActiveEventMap.end() )
    {
      if ( pEvent->timerId() == it.value().mTimerID )
      {
        mCbHandler.processTimerEventID( it.value().mEventID, it.value().mTimerID );
        it = mActiveEventMap.erase( it );
      }
      else
      {
        ++it;
      }
    }
  }
}
