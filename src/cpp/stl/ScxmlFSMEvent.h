#pragma once

#include "IScxmlFSMEvent.h"
#include "IScxmlFSMEventCB.h"
#include <list>
#include <map>

class ScxmlFSMEventInfo
{
private:
  ScxmlFSMEventInfo() {}

public:
  ScxmlFSMEventInfo( int eventID, int timerID )
    : mEventID( eventID )
    , mTimerID( timerID )
  {
  }

  ~ScxmlFSMEventInfo() {}

public:
  int mEventID;
  int mTimerID;
};

class ScxmlFSMEvent : public IScxmlFSMEvent
{
public:
  ScxmlFSMEvent( IScxmlFSMEventCB& );
  ~ScxmlFSMEvent() override;
  void             setEventID( int eventID ) override;
  int              sendEventID( const std::string& sendId, int eventID, int delayMs ) override;
  std::vector<int> cancelEvent( const std::string& sendID ) override;
  void             timerEvent( int timerID );

private:
  ScxmlFSMEvent();
  ScxmlFSMEvent& operator=( const ScxmlFSMEvent& );
  ScxmlFSMEvent( const ScxmlFSMEvent& );

  IScxmlFSMEventCB&                             mCbHandler;
  std::map<int, int>                            mEventMap;
  std::multimap<std::string, ScxmlFSMEventInfo> mActiveEventMap;
  int                                           mTimerID = 1;
};
