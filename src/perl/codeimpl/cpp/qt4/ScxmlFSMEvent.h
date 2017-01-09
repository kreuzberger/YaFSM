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
  public:
    ScxmlFSMEventInfo()
    : mDelayMs(-1)
    {
    }

    ScxmlFSMEventInfo( int delayMs )
    : mDelayMs(delayMs)
    {
    }

    ~ScxmlFSMEventInfo() {}

  public:
    int mDelayMs;
};


class ScxmlFSMEvent: public QObject, public IScxmlFSMEvent
{
  Q_OBJECT
public:
  ScxmlFSMEvent( IScxmlFSMEventCB& );
  virtual ~ScxmlFSMEvent();
  virtual void setEventID( int eventID);
  virtual void sendEventID( int eventID, int delayMs );
  virtual void cancelEventID( int eventID );
  virtual void timerEvent( QTimerEvent* );

private:
  ScxmlFSMEvent();
  ScxmlFSMEvent& operator=(const ScxmlFSMEvent&);
  ScxmlFSMEvent(const ScxmlFSMEvent&);

  IScxmlFSMEventCB& mCbHandler;
  QMap<int,ScxmlFSMEventInfo> mEventMap;
  QMap<int,int> mActiveEventMap;
};


#endif
 
