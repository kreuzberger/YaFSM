#ifndef SCXMLFSMEVENT_H
#define SCXMLFSMEVENT_H

#include "IScxmlFSMEvent.h"
#include "IScxmlFSMEventCB.h"
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>

class QTimerEvent;

class ScxmlFSMEventInfo
{
  private:
    ScxmlFSMEventInfo()
    {
    }

public:
    ScxmlFSMEventInfo( int eventID, int timerID )
      : mEventID(eventID)
      , mTimerID(timerID)
    {
    }

    ~ScxmlFSMEventInfo() {}

  public:
    int mEventID;
    int mTimerID;
};


class ScxmlFSMEvent: public QObject, public IScxmlFSMEvent
{
  Q_OBJECT
public:
  ScxmlFSMEvent( IScxmlFSMEventCB& );
  virtual ~ScxmlFSMEvent();
  virtual void setEventID( int eventID);
  virtual int sendEventID( const std::string& sendId, int eventID, int delayMs );
  virtual std::vector<int> cancelEvent( const std::string& sendID );
  virtual void timerEvent( QTimerEvent* );

private:
  ScxmlFSMEvent();
  ScxmlFSMEvent& operator=(const ScxmlFSMEvent&);
  ScxmlFSMEvent(const ScxmlFSMEvent&);

  IScxmlFSMEventCB& mCbHandler;
  QMap<int,int> mEventMap;
  QMap<std::string,ScxmlFSMEventInfo> mActiveEventMap;
};


#endif
 
