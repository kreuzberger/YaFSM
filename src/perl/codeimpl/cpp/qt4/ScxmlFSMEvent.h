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
    {
    }

    ScxmlFSMEventInfo( int delayMs )
    {
    }

    ~ScxmlFSMEventInfo() {}

  public:
};


class ScxmlFSMEvent: public QObject, public IScxmlFSMEvent
{
  Q_OBJECT
public:
  ScxmlFSMEvent( IScxmlFSMEventCB& );
  virtual ~ScxmlFSMEvent();
  virtual void setEventID( int eventID);
  virtual int sendEventID( int eventID, int delayMs );
  virtual void cancelEvent( int sendID );
  virtual void timerEvent( QTimerEvent* );

private:
  ScxmlFSMEvent();
  ScxmlFSMEvent& operator=(const ScxmlFSMEvent&);
  ScxmlFSMEvent(const ScxmlFSMEvent&);

  IScxmlFSMEventCB& mCbHandler;
  QMap<int,int> mEventMap;
  QMap<int,int> mActiveEventMap;
};


#endif
 
