#include <thread>
#include <functional>

#include <chrono>
#include <iostream>

template <class callable, class instance, class... arguments> void timer( int after, callable&& f, instance* inst, arguments&&... args )
{
  auto task( std::bind( std::forward<callable>( f ), inst, std::forward<arguments>( args )... ) );
  after = ( 0 == after ) ? 1 : after;

  std::thread( [after, task]() {
    std::this_thread::sleep_for( std::chrono::milliseconds( after ) );
    task();
  } )
    .detach();
}

//void hello() {std::cout << "Hello!\n";}

/*
 * int main()
{
       timer(std::chrono::seconds(5), &hello);
       std::cout << "Launched\n";
       std::this_thread::sleep_for(std::chrono::seconds(10));
} 
*/
#include "ScxmlFSMEvent.h"
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
  : IScxmlFSMEvent()
  , mCbHandler( handler )
  , mActiveEventMap()
{
}

ScxmlFSMEvent::~ScxmlFSMEvent()
{
}

void ScxmlFSMEvent::setEventID( int eventID )
{
  if ( mEventMap.end() == mEventMap.find( eventID ) )
  {
    mEventMap.emplace( eventID, 0 );
  }
}

int ScxmlFSMEvent::sendEventID( const std::string& sendId, int eventID, int delayMs )
{
  int id = 0;
  if ( mEventMap.end() != mEventMap.find( eventID ) )
  {
    ++mTimerID;
    timer( delayMs, &ScxmlFSMEvent::timerEvent, this, mTimerID );
    ScxmlFSMEventInfo eventInfo( eventID, mTimerID );
    mActiveEventMap.emplace( sendId, eventInfo );
    id = mTimerID;
  }
  assert( 0 < id );
  return id;
}

std::vector<int> ScxmlFSMEvent::cancelEvent( const std::string& sendId )
{
  std::vector<int> canceledEvents;
  if ( mActiveEventMap.end() != mActiveEventMap.find( sendId ) )
  {
    auto it = mActiveEventMap.find( sendId );
    while ( it != mActiveEventMap.end() && it->first == sendId )
    {
      // todo  killTimer( it->second.mTimerID );
      canceledEvents.push_back( it->second.mTimerID );
      ++it;
    }
    // todo mActiveEventMap.remove( sendId );
  }
  return canceledEvents;
}

void ScxmlFSMEvent::timerEvent( int timerID )
{
  auto it = mActiveEventMap.begin();
  while ( it != mActiveEventMap.end() )
  {
    if ( timerID == it->second.mTimerID )
    {
      mCbHandler.processTimerEventID( it->second.mEventID, it->second.mTimerID );
      it = mActiveEventMap.erase( it );
    }
    else
    {
      ++it;
    }
  }
}
